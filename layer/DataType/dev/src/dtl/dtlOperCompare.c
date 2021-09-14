/*******************************************************************************
 * dtlOperCompare.c
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
 * @file dtlOperCompare.c
 * @brief Data Type Layer Compare 함수 
 */

#include <dtl.h>
#include <dtDef.h>

/**
 * @addtogroup dtlOperCompare
 * @{
 */

/*******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

/**
 * @brief DataType Group의 대표 타입
 */
const dtlDataType dtlGroupRepresentType[ DTL_GROUP_MAX ] =
{
    DTL_TYPE_BOOLEAN,                    /**< DTL_GROUP_BOOLEAN */
    DTL_TYPE_NATIVE_BIGINT,              /**< DTL_GROUP_BINARY_INTEGER  */
    DTL_TYPE_NUMBER,                     /**< DTL_GROUP_NUMBER */
    DTL_TYPE_NATIVE_DOUBLE,              /**< DTL_GROUP_BINARY_REAL */

    DTL_TYPE_VARCHAR,                    /**< DTL_GROUP_CHAR_STRING ( DTL_TYPE_CLOB은 제외 ) */    
    DTL_TYPE_LONGVARCHAR,                /**< DTL_GROUP_LONGVARCHAR_STRING */    
    DTL_TYPE_VARBINARY,                  /**< DTL_GROUP_BINARY_STRING ( DTL_TYPE_BLOB은 제외 ) */
    DTL_TYPE_LONGVARBINARY,              /**< DTL_GROUP_LONGVARBINARY_STRING */    
    
    DTL_TYPE_DATE,                       /**< DTL_GROUP_DATE */
    DTL_TYPE_TIME,                       /**< DTL_GROUP_TIME */
    DTL_TYPE_TIMESTAMP,                  /**< DTL_GROUP_TIMESTAMP */
    DTL_TYPE_TIME_WITH_TIME_ZONE,        /**< DTL_GROUP_TIME_WITH_TIME_ZONE */
    DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,   /**< DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
    DTL_TYPE_INTERVAL_YEAR_TO_MONTH,     /**< DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
    DTL_TYPE_INTERVAL_DAY_TO_SECOND,     /**< DTL_GROUP_INTERVAL_DAY_TO_SECOND */
    
    DTL_TYPE_ROWID                       /**< DTL_GROUP_ROWID */
};

        
/**
 * @brief Datatype 간의 cast & compare 함수를 구성하기 위해 그룹화 함수 
 */
const dtlGroup dtlDataTypeGroup[DTL_TYPE_MAX] =
{ 
    DTL_GROUP_BOOLEAN,                    /* BOOLEAN */
    DTL_GROUP_BINARY_INTEGER,             /* SMALLINT */
    DTL_GROUP_BINARY_INTEGER,             /* INTEGER */
    DTL_GROUP_BINARY_INTEGER,             /* BIGINT */
    
    DTL_GROUP_BINARY_REAL,                /* REAL */
    DTL_GROUP_BINARY_REAL,                /* DOUBLE */

    DTL_GROUP_NUMBER,                     /* FLOAT */
    DTL_GROUP_NUMBER,                     /* NUMBER */
    DTL_GROUP_NUMBER,                     /* unsupported feature, DECIMAL */

    DTL_GROUP_CHAR_STRING,                /* CHARACTER */
    DTL_GROUP_CHAR_STRING,                /* CHARACTER VARYING */
    DTL_GROUP_LONGVARCHAR_STRING,         /* CHARACTER LONG VARYING */
    DTL_GROUP_CHAR_STRING,                /* unsupported feature, CHARACTER LARGE OBJECT */

    DTL_GROUP_BINARY_STRING,              /* BINARY */ 
    DTL_GROUP_BINARY_STRING,              /* BINARY VARYING */ 
    DTL_GROUP_LONGVARBINARY_STRING,       /* BINARY LONG VARYING */ 
    DTL_GROUP_BINARY_STRING,              /* unsupported feature, BINARY LARGE OBJECT */ 

    DTL_GROUP_DATE,                       /* DATE */
    DTL_GROUP_TIME,                       /* TIME WITHOUT TIME ZONE */
    DTL_GROUP_TIMESTAMP,                  /* TIMESTAMP WITHOUT TIME ZONE */
    DTL_GROUP_TIME_WITH_TIME_ZONE,        /* TIME WIT TIME ZONE */
    DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,   /* TIMESTAMP WITH TIME ZONE */

    DTL_GROUP_INTERVAL_YEAR_TO_MONTH,     /**< INTERVAL YEAR TO MONTH */
    DTL_GROUP_INTERVAL_DAY_TO_SECOND,     /**< INTERVAL DAY TO SECOND */

    DTL_GROUP_ROWID                       /* ROWID */   
};


/**
 * @brief type과 constant 정보를 이용한 비교 연산에 대한 arguments type 정보
 *   <BR> => array 구성 : [ Type_Left ][ Type_Right ][ IsConstant_Left ][ IsConstant_Right ]
 */
const dtlCompareType dtlCompOperArgType[ DTL_TYPE_MAX ][ DTL_TYPE_MAX ][ 2 ][ 2 ] =
{
    /**< BOOLEAN */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } },
          { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< INTEGER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BIGINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< NUMBER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } },
          { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } },
          { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } },
          { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< NATIVE_SMALLINT */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_SMALLINT } },
          { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_SMALLINT } } },

        /**< INTEGER */
        { { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_INTEGER } },
          { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_INTEGER } } },

        /**< BIGINT */
        { { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_BIGINT } },
          { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_BIGINT } } },

        /**< REAL */
        { { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_REAL } },
          { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_REAL } } },

        /**< DOUBLE */
        { { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_DOUBLE } },
          { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NATIVE_DOUBLE } } },

        /**< FLOAT */
        { { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_FLOAT },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_FLOAT } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT },
            { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT } } },

        /**< NUMBER */
        { { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< CHARACTER */
        { { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },
    
        /**< CHARACTER VARYING */
        { { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_SMALLINT, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< NATIVE_INTEGER */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_SMALLINT } },
          { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_SMALLINT } } },

        /**< INTEGER */
        { { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_INTEGER } },
          { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_INTEGER } } },

        /**< BIGINT */
        { { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_BIGINT } },
          { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_BIGINT } } },

        /**< REAL */
        { { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_REAL } },
          { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_REAL } } },

        /**< DOUBLE */
        { { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_DOUBLE } },
          { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NATIVE_DOUBLE } } },

        /**< FLOAT */
        { { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_FLOAT },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_FLOAT } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT },
            { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT } } },

        /**< NUMBER */
        { { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< CHARACTER */
        { { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },
    
        /**< CHARACTER VARYING */
        { { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< NATIVE_BIGINT */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_SMALLINT } },
          { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_SMALLINT } } },

        /**< INTEGER */
        { { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_INTEGER } },
          { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_INTEGER } } },

        /**< BIGINT */
        { { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_BIGINT } },
          { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_BIGINT } } },

        /**< REAL */
        { { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_REAL } },
          { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_REAL } } },

        /**< DOUBLE */
        { { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_DOUBLE } },
          { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NATIVE_DOUBLE } } },

        /**< FLOAT */
        { { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_FLOAT },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_FLOAT } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT },
            { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT } } },

        /**< NUMBER */
        { { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< CHARACTER */
        { { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },
    
        /**< CHARACTER VARYING */
        { { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_BIGINT, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< NATIVE_REAL */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_SMALLINT } },
          { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_SMALLINT } } },

        /**< INTEGER */
        { { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_INTEGER } },
          { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_INTEGER } } },

        /**< BIGINT */
        { { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_BIGINT } },
          { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_BIGINT } } },

        /**< REAL */
        { { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_REAL } },
          { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_REAL } } },

        /**< DOUBLE */
        { { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_DOUBLE } },
          { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NATIVE_DOUBLE } } },

        /**< FLOAT */
        { { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_FLOAT },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_FLOAT } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT },
            { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },
    
        /**< CHARACTER VARYING */
        { { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_REAL, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< NATIVE_DOUBLE  */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_SMALLINT } },
          { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_SMALLINT } } },

        /**< INTEGER */
        { { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_INTEGER } },
          { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_INTEGER } } },

        /**< BIGINT */
        { { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_BIGINT } },
          { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_BIGINT } } },

        /**< REAL */
        { { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_REAL } },
          { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_REAL } } },

        /**< DOUBLE */
        { { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_DOUBLE } },
          { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NATIVE_DOUBLE } } },

        /**< FLOAT */
        { { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_FLOAT },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_FLOAT } },
          { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_FLOAT },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_FLOAT } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER } } },
    
        /**< CHARACTER VARYING */
        { { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER },
            { DTL_TYPE_NATIVE_DOUBLE, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< FLOAT */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_FLOAT,  DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } },
          { { DTL_TYPE_FLOAT,  DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } } },

        /**< INTEGER */
        { { { DTL_TYPE_FLOAT,  DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } },
          { { DTL_TYPE_FLOAT,  DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } } },

        /**< BIGINT */
        { { { DTL_TYPE_FLOAT,  DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } },
          { { DTL_TYPE_FLOAT,  DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } } },

        /**< REAL */
        { { { DTL_TYPE_FLOAT,  DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } },
          { { DTL_TYPE_FLOAT,  DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } } },

        /**< DOUBLE */
        { { { DTL_TYPE_FLOAT,  DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NATIVE_DOUBLE } },
          { { DTL_TYPE_FLOAT,  DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NATIVE_DOUBLE } } },

        /**< FLOAT */
        { { { DTL_TYPE_FLOAT,  DTL_TYPE_FLOAT },
            { DTL_TYPE_FLOAT,  DTL_TYPE_FLOAT } },
          { { DTL_TYPE_FLOAT,  DTL_TYPE_FLOAT },
            { DTL_TYPE_FLOAT,  DTL_TYPE_FLOAT } } },

        /**< NUMBER */
        { { { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } },
          { { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } },
          { { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } },
          { { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } },
          { { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER },
            { DTL_TYPE_FLOAT,  DTL_TYPE_NUMBER } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },
    
    /**< NUMBER */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< INTEGER */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< BIGINT */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< REAL */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< DOUBLE */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE } } },

        /**< FLOAT */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT },
            { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT },
            { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },
    
    /**< DECIMAL */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< INTEGER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BIGINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< NUMBER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< CHARACTER */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } },
          { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< INTEGER */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< BIGINT */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< REAL */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< DOUBLE */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE } } },

        /**< FLOAT */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT },
            { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT },
            { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_CHAR, DTL_TYPE_CHAR },
            { DTL_TYPE_CHAR, DTL_TYPE_CHAR } },
          { { DTL_TYPE_CHAR, DTL_TYPE_CHAR },
            { DTL_TYPE_CHAR, DTL_TYPE_CHAR } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_CHAR, DTL_TYPE_VARCHAR },
            { DTL_TYPE_CHAR, DTL_TYPE_VARCHAR } },
          { { DTL_TYPE_CHAR, DTL_TYPE_VARCHAR },
            { DTL_TYPE_CHAR, DTL_TYPE_VARCHAR } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_CHAR, DTL_TYPE_LONGVARCHAR },
            { DTL_TYPE_CHAR, DTL_TYPE_LONGVARCHAR } },
          { { DTL_TYPE_CHAR, DTL_TYPE_LONGVARCHAR },
            { DTL_TYPE_CHAR, DTL_TYPE_LONGVARCHAR } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_VARBINARY, DTL_TYPE_BINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_BINARY } },
          { { DTL_TYPE_VARBINARY, DTL_TYPE_BINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_BINARY } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY } },
          { { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY } },
          { { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DATE */
        { { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } },
          { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } } },
                            
        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } },
          { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } },
          { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } },
          { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } } }
    },
    
    /**< CHARACTER VARYING */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } },
          { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< INTEGER */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< BIGINT */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< REAL */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< DOUBLE */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE } } },

        /**< FLOAT */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT },
            { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT },
            { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_VARCHAR, DTL_TYPE_CHAR },
            { DTL_TYPE_VARCHAR, DTL_TYPE_CHAR } },
          { { DTL_TYPE_VARCHAR, DTL_TYPE_CHAR },
            { DTL_TYPE_VARCHAR, DTL_TYPE_CHAR } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_VARCHAR, DTL_TYPE_VARCHAR },
            { DTL_TYPE_VARCHAR, DTL_TYPE_VARCHAR } },
          { { DTL_TYPE_VARCHAR, DTL_TYPE_VARCHAR },
            { DTL_TYPE_VARCHAR, DTL_TYPE_VARCHAR } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_VARCHAR, DTL_TYPE_LONGVARCHAR },
            { DTL_TYPE_VARCHAR, DTL_TYPE_LONGVARCHAR } },
          { { DTL_TYPE_VARCHAR, DTL_TYPE_LONGVARCHAR },
            { DTL_TYPE_VARCHAR, DTL_TYPE_LONGVARCHAR } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_VARBINARY, DTL_TYPE_BINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_BINARY } },
          { { DTL_TYPE_VARBINARY, DTL_TYPE_BINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_BINARY } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY } },
          { { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY } },
          { { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DATE */
        { { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } },
          { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } } },
                            
        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } },
          { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } },
          { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } },
          { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } } }
    },

    /**< CHARACTER LONG VARYING */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } },
          { { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN },
            { DTL_TYPE_BOOLEAN, DTL_TYPE_BOOLEAN } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_SMALLINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< INTEGER */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_INTEGER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< BIGINT */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_BIGINT },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< REAL */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_REAL },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE },
            { DTL_TYPE_NUMBER, DTL_TYPE_NATIVE_DOUBLE } } },

        /**< FLOAT */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT },
            { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT },
            { DTL_TYPE_NUMBER, DTL_TYPE_FLOAT } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } },
          { { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER },
            { DTL_TYPE_NUMBER, DTL_TYPE_NUMBER } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_LONGVARCHAR, DTL_TYPE_CHAR },
            { DTL_TYPE_LONGVARCHAR, DTL_TYPE_CHAR } },
          { { DTL_TYPE_LONGVARCHAR, DTL_TYPE_CHAR },
            { DTL_TYPE_LONGVARCHAR, DTL_TYPE_CHAR } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_LONGVARCHAR, DTL_TYPE_VARCHAR },
            { DTL_TYPE_LONGVARCHAR, DTL_TYPE_VARCHAR } },
          { { DTL_TYPE_LONGVARCHAR, DTL_TYPE_VARCHAR },
            { DTL_TYPE_LONGVARCHAR, DTL_TYPE_VARCHAR } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_LONGVARCHAR, DTL_TYPE_LONGVARCHAR },
            { DTL_TYPE_LONGVARCHAR, DTL_TYPE_LONGVARCHAR } },
          { { DTL_TYPE_LONGVARCHAR, DTL_TYPE_LONGVARCHAR },
            { DTL_TYPE_LONGVARCHAR, DTL_TYPE_LONGVARCHAR } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_BINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_BINARY } },
          { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_BINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_BINARY } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY } },
          { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_LONGVARBINARY } },
          { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_LONGVARBINARY } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } },
          { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } },
          { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } },
          { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } },
          { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } } }
    },

    /**< CHARACTER LARGE OBJECT */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< INTEGER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BIGINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },
    
    /**< BINARY */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< INTEGER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BIGINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_BINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_BINARY, DTL_TYPE_VARBINARY } },
          { { DTL_TYPE_BINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_BINARY, DTL_TYPE_VARBINARY } } },
        
        /**< CHARACTER VARYING */
        { { { DTL_TYPE_BINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_BINARY, DTL_TYPE_VARBINARY } },
          { { DTL_TYPE_BINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_BINARY, DTL_TYPE_VARBINARY } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_BINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_BINARY, DTL_TYPE_LONGVARBINARY } },
          { { DTL_TYPE_BINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_BINARY, DTL_TYPE_LONGVARBINARY } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_BINARY, DTL_TYPE_BINARY },
            { DTL_TYPE_BINARY, DTL_TYPE_BINARY } },
          { { DTL_TYPE_BINARY, DTL_TYPE_BINARY },
            { DTL_TYPE_BINARY, DTL_TYPE_BINARY } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_BINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_BINARY, DTL_TYPE_VARBINARY } },
          { { DTL_TYPE_BINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_BINARY, DTL_TYPE_VARBINARY } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_BINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_BINARY, DTL_TYPE_LONGVARBINARY } },
          { { DTL_TYPE_BINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_BINARY, DTL_TYPE_LONGVARBINARY } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },
    
    /**< BINARY VARYING */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< INTEGER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BIGINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY } },
          { { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY } } },
        
        /**< CHARACTER VARYING */
        { { { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY } },
          { { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY } },
          { { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_VARBINARY, DTL_TYPE_BINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_BINARY } },
          { { DTL_TYPE_VARBINARY, DTL_TYPE_BINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_BINARY } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY } },
          { { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_VARBINARY } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY } },
          { { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_VARBINARY, DTL_TYPE_LONGVARBINARY } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< BINARY LONG VARYING */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< INTEGER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BIGINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY } },
          { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY } } },
        
        /**< CHARACTER VARYING */
        { { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY } },
          { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_LONGVARBINARY } },
          { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_LONGVARBINARY } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_BINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_BINARY } },
          { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_BINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_BINARY } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY } },
          { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_VARBINARY } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_LONGVARBINARY } },
          { { DTL_TYPE_LONGVARBINARY, DTL_TYPE_LONGVARBINARY },
            { DTL_TYPE_LONGVARBINARY, DTL_TYPE_LONGVARBINARY } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< BINARY LARGE OBJECT */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< INTEGER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BIGINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< DATE */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< INTEGER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BIGINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } },
          { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } },
          { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } },
          { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } },
          { { DTL_TYPE_DATE, DTL_TYPE_DATE },
            { DTL_TYPE_DATE, DTL_TYPE_DATE } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_DATE, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_DATE, DTL_TYPE_TIMESTAMP } },
          { { DTL_TYPE_DATE, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_DATE, DTL_TYPE_TIMESTAMP } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< TIME WITHOUT TIME ZONE */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< INTEGER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BIGINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } },
          { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } },
          { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } },
          { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } },
          { { DTL_TYPE_TIME, DTL_TYPE_TIME },
            { DTL_TYPE_TIME, DTL_TYPE_TIME } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< TIMESTAMP WITHOUT TIME ZONE */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< INTEGER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BIGINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } },
          { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } },
          { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } },
          { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_TIMESTAMP, DTL_TYPE_DATE },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_DATE } },
          { { DTL_TYPE_TIMESTAMP, DTL_TYPE_DATE },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_DATE } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } },
          { { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP },
            { DTL_TYPE_TIMESTAMP, DTL_TYPE_TIMESTAMP } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< TIME WITH TIME ZONE */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< INTEGER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BIGINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE },
            { DTL_TYPE_TIME_WITH_TIME_ZONE, DTL_TYPE_TIME_WITH_TIME_ZONE } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },
    
    /**< TIMESTAMP WITH TIME ZONE */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< INTEGER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BIGINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< CHARACTER */
        { { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } },
          { { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE },
            { DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE } } },
                            
        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< INTERVAL YEAR_TO_MONTH */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< INTEGER */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< BIGINT */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< NUMBER */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< CHARACTER */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } },
          { { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH },
            { DTL_TYPE_INTERVAL_YEAR_TO_MONTH, DTL_TYPE_INTERVAL_YEAR_TO_MONTH } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< INTERVAL DAY_TO_SECOND */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< INTEGER */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< BIGINT */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< CHARACTER */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } },
          { { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND },
            { DTL_TYPE_INTERVAL_DAY_TO_SECOND, DTL_TYPE_INTERVAL_DAY_TO_SECOND } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } }
    },

    /**< ROWID */
    {
        /**< BOOLEAN */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< SMALLINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< INTEGER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BIGINT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< REAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DOUBLE PRECISION */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< FLOAT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
        
        /**< NUMBER */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, DECIMAL */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< CHARACTER */
        { { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } },
          { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } } },

        /**< CHARACTER VARYING */
        { { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } },
          { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } } },

        /**< CHARACTER LONG VARYING */
        { { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } },
          { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } } },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
    
        /**< BINARY */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< BINARY LONG VARYING */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< unsupported feature, BINARY LARGE OBJECT */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },
                            
        /**< DATE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIME WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< TIMESTAMP WITH TIME ZONE */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        { { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } },
          { { DTL_TYPE_NA, DTL_TYPE_NA },
            { DTL_TYPE_NA, DTL_TYPE_NA } } },

        /**< DTL_TYPE_ROWID */
        { { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } },
          { { DTL_TYPE_ROWID, DTL_TYPE_ROWID },
            { DTL_TYPE_ROWID, DTL_TYPE_ROWID } } }
    }    
};


/*******************************************************************************
 * DECLARE FUNCTION POINTER
 ******************************************************************************/

dtlBuiltInFuncPtr dtlCompOperArg1FuncList[ DTL_COMP_OPER_MAX - DTL_COMP_OPER_IS_NULL ] =
{
    dtlOperIsNull,          /**< IS NULL */
    dtlOperIsNotNull        /**< IS NOT NULL */
};

dtlBuiltInFuncPtr dtlCompOperArg2FuncList[ DTL_TYPE_MAX ][ DTL_TYPE_MAX ][ DTL_COMP_OPER_IS_NULL ] =
{
    /*******************************************************************************
     * BOOLEAN
     ******************************************************************************/
    /**< BOOLEAN */
    {
    
        /**< BOOLEAN */
        {   
            dtlOperIsEqualBooleanToBoolean,              /**< IS EQUAL */
            dtlOperIsNotEqualBooleanToBoolean,           /**< IS NOT EQUAL */
            dtlOperIsLessThanBooleanToBoolean,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualBooleanToBoolean,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanBooleanToBoolean,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualBooleanToBoolean,   /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

    /*******************************************************************************
     * SMALLINT
     ******************************************************************************/
    /**< SMALLINT */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            dtlOperIsEqualSmallIntToSmallInt,              /**< IS EQUAL */
            dtlOperIsNotEqualSmallIntToSmallInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanSmallIntToSmallInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualSmallIntToSmallInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanSmallIntToSmallInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualSmallIntToSmallInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlOperIsEqualSmallIntToInteger,              /**< IS EQUAL */
            dtlOperIsNotEqualSmallIntToInteger,           /**< IS NOT EQUAL */
            dtlOperIsLessThanSmallIntToInteger,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualSmallIntToInteger,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanSmallIntToInteger,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualSmallIntToInteger,   /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlOperIsEqualSmallIntToBigInt,              /**< IS EQUAL */
            dtlOperIsNotEqualSmallIntToBigInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanSmallIntToBigInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualSmallIntToBigInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanSmallIntToBigInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualSmallIntToBigInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            dtlOperIsEqualSmallIntToReal,              /**< IS EQUAL */
            dtlOperIsNotEqualSmallIntToReal,           /**< IS NOT EQUAL */
            dtlOperIsLessThanSmallIntToReal,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualSmallIntToReal,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanSmallIntToReal,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualSmallIntToReal,   /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlOperIsEqualSmallIntToDouble,              /**< IS EQUAL */
            dtlOperIsNotEqualSmallIntToDouble,           /**< IS NOT EQUAL */
            dtlOperIsLessThanSmallIntToDouble,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualSmallIntToDouble,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanSmallIntToDouble,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualSmallIntToDouble,   /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlOperIsEqualSmallIntToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualSmallIntToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanSmallIntToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualSmallIntToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanSmallIntToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualSmallIntToNumeric,   /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            dtlOperIsEqualSmallIntToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualSmallIntToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanSmallIntToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualSmallIntToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanSmallIntToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualSmallIntToNumeric,   /**< IS GREATER THAN EQUAL */
        },
        
        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * INTEGER
 ******************************************************************************/
    /**< INTEGER */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            dtlOperIsEqualIntegerToSmallInt,              /**< IS EQUAL */
            dtlOperIsNotEqualIntegerToSmallInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanIntegerToSmallInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualIntegerToSmallInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanIntegerToSmallInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualIntegerToSmallInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlOperIsEqualIntegerToInteger,              /**< IS EQUAL */
            dtlOperIsNotEqualIntegerToInteger,           /**< IS NOT EQUAL */
            dtlOperIsLessThanIntegerToInteger,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualIntegerToInteger,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanIntegerToInteger,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualIntegerToInteger,   /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlOperIsEqualIntegerToBigInt,              /**< IS EQUAL */
            dtlOperIsNotEqualIntegerToBigInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanIntegerToBigInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualIntegerToBigInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanIntegerToBigInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualIntegerToBigInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            dtlOperIsEqualIntegerToReal,              /**< IS EQUAL */
            dtlOperIsNotEqualIntegerToReal,           /**< IS NOT EQUAL */
            dtlOperIsLessThanIntegerToReal,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualIntegerToReal,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanIntegerToReal,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualIntegerToReal,   /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlOperIsEqualIntegerToDouble,              /**< IS EQUAL */
            dtlOperIsNotEqualIntegerToDouble,           /**< IS NOT EQUAL */
            dtlOperIsLessThanIntegerToDouble,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualIntegerToDouble,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanIntegerToDouble,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualIntegerToDouble,   /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlOperIsEqualIntegerToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualIntegerToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanIntegerToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualIntegerToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanIntegerToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualIntegerToNumeric,   /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            dtlOperIsEqualIntegerToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualIntegerToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanIntegerToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualIntegerToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanIntegerToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualIntegerToNumeric,   /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * BIGINT
 ******************************************************************************/
    /**< BIGINT */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            dtlOperIsEqualBigIntToSmallInt,              /**< IS EQUAL */
            dtlOperIsNotEqualBigIntToSmallInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanBigIntToSmallInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualBigIntToSmallInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanBigIntToSmallInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualBigIntToSmallInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlOperIsEqualBigIntToInteger,              /**< IS EQUAL */
            dtlOperIsNotEqualBigIntToInteger,           /**< IS NOT EQUAL */
            dtlOperIsLessThanBigIntToInteger,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualBigIntToInteger,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanBigIntToInteger,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualBigIntToInteger,   /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlOperIsEqualBigIntToBigInt,              /**< IS EQUAL */
            dtlOperIsNotEqualBigIntToBigInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanBigIntToBigInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualBigIntToBigInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanBigIntToBigInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualBigIntToBigInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            dtlOperIsEqualBigIntToReal,              /**< IS EQUAL */
            dtlOperIsNotEqualBigIntToReal,           /**< IS NOT EQUAL */
            dtlOperIsLessThanBigIntToReal,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualBigIntToReal,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanBigIntToReal,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualBigIntToReal,   /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlOperIsEqualBigIntToDouble,              /**< IS EQUAL */
            dtlOperIsNotEqualBigIntToDouble,           /**< IS NOT EQUAL */
            dtlOperIsLessThanBigIntToDouble,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualBigIntToDouble,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanBigIntToDouble,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualBigIntToDouble,   /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlOperIsEqualBigIntToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualBigIntToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanBigIntToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualBigIntToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanBigIntToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualBigIntToNumeric,   /**< IS GREATER THAN EQUAL */
        },

        /**< NUMBER */
        {   
            dtlOperIsEqualBigIntToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualBigIntToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanBigIntToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualBigIntToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanBigIntToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualBigIntToNumeric,   /**< IS GREATER THAN EQUAL */
        },
       
        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * REAL
 ******************************************************************************/
    /**< REAL */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            dtlOperIsEqualRealToSmallInt,              /**< IS EQUAL */
            dtlOperIsNotEqualRealToSmallInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanRealToSmallInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualRealToSmallInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanRealToSmallInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualRealToSmallInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlOperIsEqualRealToInteger,              /**< IS EQUAL */
            dtlOperIsNotEqualRealToInteger,           /**< IS NOT EQUAL */
            dtlOperIsLessThanRealToInteger,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualRealToInteger,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanRealToInteger,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualRealToInteger,   /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlOperIsEqualRealToBigInt,              /**< IS EQUAL */
            dtlOperIsNotEqualRealToBigInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanRealToBigInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualRealToBigInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanRealToBigInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualRealToBigInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            dtlOperIsEqualRealToReal,              /**< IS EQUAL */
            dtlOperIsNotEqualRealToReal,           /**< IS NOT EQUAL */
            dtlOperIsLessThanRealToReal,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualRealToReal,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanRealToReal,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualRealToReal,   /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlOperIsEqualRealToDouble,              /**< IS EQUAL */
            dtlOperIsNotEqualRealToDouble,           /**< IS NOT EQUAL */
            dtlOperIsLessThanRealToDouble,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualRealToDouble,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanRealToDouble,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualRealToDouble,   /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlOperIsEqualRealToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualRealToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanRealToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualRealToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanRealToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualRealToNumeric,   /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            dtlOperIsEqualRealToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualRealToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanRealToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualRealToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanRealToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualRealToNumeric,   /**< IS GREATER THAN EQUAL */
        },
        
        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * DOUBLE
 ******************************************************************************/
    /**< DOUBLE */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            dtlOperIsEqualDoubleToSmallInt,              /**< IS EQUAL */
            dtlOperIsNotEqualDoubleToSmallInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanDoubleToSmallInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualDoubleToSmallInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanDoubleToSmallInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualDoubleToSmallInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlOperIsEqualDoubleToInteger,              /**< IS EQUAL */
            dtlOperIsNotEqualDoubleToInteger,           /**< IS NOT EQUAL */
            dtlOperIsLessThanDoubleToInteger,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualDoubleToInteger,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanDoubleToInteger,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualDoubleToInteger,   /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlOperIsEqualDoubleToBigInt,              /**< IS EQUAL */
            dtlOperIsNotEqualDoubleToBigInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanDoubleToBigInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualDoubleToBigInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanDoubleToBigInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualDoubleToBigInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            dtlOperIsEqualDoubleToReal,              /**< IS EQUAL */
            dtlOperIsNotEqualDoubleToReal,           /**< IS NOT EQUAL */
            dtlOperIsLessThanDoubleToReal,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualDoubleToReal,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanDoubleToReal,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualDoubleToReal,   /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlOperIsEqualDoubleToDouble,              /**< IS EQUAL */
            dtlOperIsNotEqualDoubleToDouble,           /**< IS NOT EQUAL */
            dtlOperIsLessThanDoubleToDouble,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualDoubleToDouble,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanDoubleToDouble,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualDoubleToDouble,   /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlOperIsEqualDoubleToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualDoubleToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanDoubleToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualDoubleToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanDoubleToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualDoubleToNumeric,   /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            dtlOperIsEqualDoubleToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualDoubleToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanDoubleToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualDoubleToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanDoubleToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualDoubleToNumeric,   /**< IS GREATER THAN EQUAL */
        },
       
        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * FLOAT
 ******************************************************************************/
    /**< FLOAT */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            dtlOperIsEqualNumericToSmallInt,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToSmallInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToSmallInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToSmallInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToSmallInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToSmallInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlOperIsEqualNumericToInteger,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToInteger,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToInteger,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToInteger,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToInteger,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToInteger,   /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlOperIsEqualNumericToBigInt,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToBigInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToBigInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToBigInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToBigInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToBigInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            dtlOperIsEqualNumericToReal,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToReal,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToReal,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToReal,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToReal,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToReal,   /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlOperIsEqualNumericToDouble,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToDouble,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToDouble,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToDouble,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToDouble,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToDouble,   /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlOperIsEqualNumericToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToNumeric,   /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            dtlOperIsEqualNumericToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToNumeric,   /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },
    
/*******************************************************************************
 * NUMBER
 ******************************************************************************/
    /**< NUMBER */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            dtlOperIsEqualNumericToSmallInt,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToSmallInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToSmallInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToSmallInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToSmallInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToSmallInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlOperIsEqualNumericToInteger,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToInteger,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToInteger,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToInteger,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToInteger,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToInteger,   /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlOperIsEqualNumericToBigInt,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToBigInt,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToBigInt,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToBigInt,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToBigInt,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToBigInt,   /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            dtlOperIsEqualNumericToReal,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToReal,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToReal,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToReal,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToReal,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToReal,   /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlOperIsEqualNumericToDouble,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToDouble,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToDouble,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToDouble,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToDouble,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToDouble,   /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlOperIsEqualNumericToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToNumeric,   /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            dtlOperIsEqualNumericToNumeric,              /**< IS EQUAL */
            dtlOperIsNotEqualNumericToNumeric,           /**< IS NOT EQUAL */
            dtlOperIsLessThanNumericToNumeric,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualNumericToNumeric,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanNumericToNumeric,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualNumericToNumeric,   /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },
    
/*******************************************************************************
 * DECIMAL
 ******************************************************************************/
    /**< DECIMAL */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * CHARACTER
 ******************************************************************************/
    /**< CHARACTER */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            dtlOperIsEqualFixedLengthChar,              /**< IS EQUAL */
            dtlOperIsNotEqualFixedLengthChar,           /**< IS NOT EQUAL */
            dtlOperIsLessThanFixedLengthChar,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualFixedLengthChar,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanFixedLengthChar,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualFixedLengthChar,   /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            dtlOperIsEqualVariableLengthChar,              /**< IS EQUAL */
            dtlOperIsNotEqualVariableLengthChar,           /**< IS NOT EQUAL */
            dtlOperIsLessThanVariableLengthChar,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualVariableLengthChar,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanVariableLengthChar,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualVariableLengthChar,   /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            dtlOperIsEqualCharAndLongvarchar,              /**< IS EQUAL */
            dtlOperIsNotEqualCharAndLongvarchar,           /**< IS NOT EQUAL */
            dtlOperIsLessThanCharAndLongvarchar,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualCharAndLongvarchar,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanCharAndLongvarchar,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualCharAndLongvarchar,   /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * CHARACTER VARYING
 ******************************************************************************/
    /**< CHARACTER VARYING */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            dtlOperIsEqualVariableLengthChar,              /**< IS EQUAL */
            dtlOperIsNotEqualVariableLengthChar,           /**< IS NOT EQUAL */
            dtlOperIsLessThanVariableLengthChar,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualVariableLengthChar,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanVariableLengthChar,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualVariableLengthChar,   /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            dtlOperIsEqualVariableLengthChar,              /**< IS EQUAL */
            dtlOperIsNotEqualVariableLengthChar,           /**< IS NOT EQUAL */
            dtlOperIsLessThanVariableLengthChar,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualVariableLengthChar,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanVariableLengthChar,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualVariableLengthChar,   /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            dtlOperIsEqualVarcharAndLongvarchar,              /**< IS EQUAL */
            dtlOperIsNotEqualVarcharAndLongvarchar,           /**< IS NOT EQUAL */
            dtlOperIsLessThanVarcharAndLongvarchar,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualVarcharAndLongvarchar,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanVarcharAndLongvarchar,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualVarcharAndLongvarchar,   /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * CHARACTER LONG VARYING
 ******************************************************************************/
    /**< CHARACTER LONG VARYING */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            dtlOperIsEqualLongvarcharAndChar,              /**< IS EQUAL */
            dtlOperIsNotEqualLongvarcharAndChar,           /**< IS NOT EQUAL */
            dtlOperIsLessThanLongvarcharAndChar,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualLongvarcharAndChar,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanLongvarcharAndChar,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualLongvarcharAndChar,   /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            dtlOperIsEqualLongvarcharAndVarchar,              /**< IS EQUAL */
            dtlOperIsNotEqualLongvarcharAndVarchar,           /**< IS NOT EQUAL */
            dtlOperIsLessThanLongvarcharAndVarchar,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualLongvarcharAndVarchar,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanLongvarcharAndVarchar,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualLongvarcharAndVarchar,   /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            dtlOperIsEqualLongvarcharAndLongvarchar,              /**< IS EQUAL */
            dtlOperIsNotEqualLongvarcharAndLongvarchar,           /**< IS NOT EQUAL */
            dtlOperIsLessThanLongvarcharAndLongvarchar,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualLongvarcharAndLongvarchar,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanLongvarcharAndLongvarchar,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualLongvarcharAndLongvarchar,   /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * CHARACTER LARGE OBJECT
 ******************************************************************************/
    /**< CHARACTER LARGE OBJECT */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },
       
/*******************************************************************************
 * BINARY
 ******************************************************************************/
    /**< BINARY */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            dtlOperIsEqualFixedLengthBinary,              /**< IS EQUAL */
            dtlOperIsNotEqualFixedLengthBinary,           /**< IS NOT EQUAL */
            dtlOperIsLessThanFixedLengthBinary,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualFixedLengthBinary,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanFixedLengthBinary,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualFixedLengthBinary,   /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            dtlOperIsEqualVariableLengthBinary,              /**< IS EQUAL */
            dtlOperIsNotEqualVariableLengthBinary,           /**< IS NOT EQUAL */
            dtlOperIsLessThanVariableLengthBinary,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualVariableLengthBinary,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanVariableLengthBinary,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualVariableLengthBinary,   /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            dtlOperIsEqualBinaryAndLongvarbinary,              /**< IS EQUAL */
            dtlOperIsNotEqualBinaryAndLongvarbinary,           /**< IS NOT EQUAL */
            dtlOperIsLessThanBinaryAndLongvarbinary,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualBinaryAndLongvarbinary,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanBinaryAndLongvarbinary,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualBinaryAndLongvarbinary,   /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * BINARY VARYING
 ******************************************************************************/
    /**< BINARY VARYING */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            dtlOperIsEqualVariableLengthBinary,              /**< IS EQUAL */
            dtlOperIsNotEqualVariableLengthBinary,           /**< IS NOT EQUAL */
            dtlOperIsLessThanVariableLengthBinary,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualVariableLengthBinary,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanVariableLengthBinary,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualVariableLengthBinary,   /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            dtlOperIsEqualVariableLengthBinary,              /**< IS EQUAL */
            dtlOperIsNotEqualVariableLengthBinary,           /**< IS NOT EQUAL */
            dtlOperIsLessThanVariableLengthBinary,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualVariableLengthBinary,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanVariableLengthBinary,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualVariableLengthBinary,   /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            dtlOperIsEqualVarbinaryAndLongvarbinary,              /**< IS EQUAL */
            dtlOperIsNotEqualVarbinaryAndLongvarbinary,           /**< IS NOT EQUAL */
            dtlOperIsLessThanVarbinaryAndLongvarbinary,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualVarbinaryAndLongvarbinary,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanVarbinaryAndLongvarbinary,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualVarbinaryAndLongvarbinary,   /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * BINARY LONG VARYING
 ******************************************************************************/
    /**< BINARY LONG VARYING */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            dtlOperIsEqualLongvarbinaryAndBinary,              /**< IS EQUAL */
            dtlOperIsNotEqualLongvarbinaryAndBinary,           /**< IS NOT EQUAL */
            dtlOperIsLessThanLongvarbinaryAndBinary,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualLongvarbinaryAndBinary,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanLongvarbinaryAndBinary,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualLongvarbinaryAndBinary,   /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            dtlOperIsEqualLongvarbinaryAndVarbinary,              /**< IS EQUAL */
            dtlOperIsNotEqualLongvarbinaryAndVarbinary,           /**< IS NOT EQUAL */
            dtlOperIsLessThanLongvarbinaryAndVarbinary,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualLongvarbinaryAndVarbinary,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanLongvarbinaryAndVarbinary,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualLongvarbinaryAndVarbinary,   /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            dtlOperIsEqualLongvarbinaryAndLongvarbinary,              /**< IS EQUAL */
            dtlOperIsNotEqualLongvarbinaryAndLongvarbinary,           /**< IS NOT EQUAL */
            dtlOperIsLessThanLongvarbinaryAndLongvarbinary,           /**< IS LESS THAN */
            dtlOperIsLessThanEqualLongvarbinaryAndLongvarbinary,      /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanLongvarbinaryAndLongvarbinary,        /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualLongvarbinaryAndLongvarbinary,   /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * BINARY LARGE OBJECT
 ******************************************************************************/
    /**< BINARY LARGE OBJECT */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },
        
/*******************************************************************************
 * DATE
 ******************************************************************************/
    /**< DATE */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            dtlOperIsEqualDateToDate,           /**< IS EQUAL */
            dtlOperIsNotEqualDateToDate,        /**< IS NOT EQUAL */
            dtlOperIsLessThanDateToDate,        /**< IS LESS THAN */
            dtlOperIsLessThanEqualDateToDate,   /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanDateToDate,     /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualDateToDate /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {
            dtlOperIsEqualDateToTimestamp,           /**< IS EQUAL */
            dtlOperIsNotEqualDateToTimestamp,        /**< IS NOT EQUAL */
            dtlOperIsLessThanDateToTimestamp,        /**< IS LESS THAN */
            dtlOperIsLessThanEqualDateToTimestamp,   /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanDateToTimestamp,     /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualDateToTimestamp /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * TIME WITHOUT TIME ZONE
 ******************************************************************************/
    /**< TIME WITHOUT TIME ZONE */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {
            dtlOperIsEqualTimeToTime,           /**< IS EQUAL */
            dtlOperIsNotEqualTimeToTime,        /**< IS NOT EQUAL */
            dtlOperIsLessThanTimeToTime,        /**< IS LESS THAN */
            dtlOperIsLessThanEqualTimeToTime,   /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanTimeToTime,     /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualTimeToTime /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * TIMESTAMP WITHOUT TIME ZONE
 ******************************************************************************/
    /**< TIMESTAMP WITHOUT TIME ZONE */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        

        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {
            dtlOperIsEqualTimestampToDate,           /**< IS EQUAL */
            dtlOperIsNotEqualTimestampToDate,        /**< IS NOT EQUAL */
            dtlOperIsLessThanTimestampToDate,        /**< IS LESS THAN */
            dtlOperIsLessThanEqualTimestampToDate,   /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanTimestampToDate,     /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualTimestampToDate /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            dtlOperIsEqualTimestampToTimestamp,           /**< IS EQUAL */
            dtlOperIsNotEqualTimestampToTimestamp,        /**< IS NOT EQUAL */
            dtlOperIsLessThanTimestampToTimestamp,        /**< IS LESS THAN */
            dtlOperIsLessThanEqualTimestampToTimestamp,   /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanTimestampToTimestamp,     /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualTimestampToTimestamp /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * TIME WITH TIME ZONE
 ******************************************************************************/
    /**< TIME WITH TIME ZONE */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            dtlOperIsEqualTimeTzToTimeTz,           /**< IS EQUAL */
            dtlOperIsNotEqualTimeTzToTimeTz,        /**< IS NOT EQUAL */
            dtlOperIsLessThanTimeTzToTimeTz,        /**< IS LESS THAN */
            dtlOperIsLessThanEqualTimeTzToTimeTz,   /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanTimeTzToTimeTz,     /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualTimeTzToTimeTz /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * TIMESTAMP WITH TIME ZONE
 ******************************************************************************/
    /**< TIMESTAMP WITH TIME ZONE */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            dtlOperIsEqualTimestampTzToTimestampTz,           /**< IS EQUAL */
            dtlOperIsNotEqualTimestampTzToTimestampTz,        /**< IS NOT EQUAL */
            dtlOperIsLessThanTimestampTzToTimestampTz,        /**< IS LESS THAN */
            dtlOperIsLessThanEqualTimestampTzToTimestampTz,   /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanTimestampTzToTimestampTz,     /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualTimestampTzToTimestampTz /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    }, 

/*******************************************************************************
 * DTL_TYPE_INTERVAL_YEAR_TO_MONTH
 ******************************************************************************/
    /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH         */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            dtlOperIsEqualIntervalYearToMonth,            /**< IS EQUAL */
            dtlOperIsNotEqualIntervalYearToMonth,         /**< IS NOT EQUAL */
            dtlOperIsLessThanIntervalYearToMonth,         /**< IS LESS THAN */
            dtlOperIsLessThanEqualIntervalYearToMonth,    /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanIntervalYearToMonth,      /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualIntervalYearToMonth  /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * DTL_TYPE_INTERVAL_DAY_TO_SECOND
 ******************************************************************************/
    /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND         */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {   
            dtlOperIsEqualIntervalDayToSecond,            /**< IS EQUAL */
            dtlOperIsNotEqualIntervalDayToSecond,         /**< IS NOT EQUAL */
            dtlOperIsLessThanIntervalDayToSecond,         /**< IS LESS THAN */
            dtlOperIsLessThanEqualIntervalDayToSecond,    /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanIntervalDayToSecond,      /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualIntervalDayToSecond  /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_ROWID */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        }
        
    },

/*******************************************************************************
 * DTL_TYPE_ROWID
 ******************************************************************************/
    /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND         */
    {
    
        /**< BOOLEAN */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< SMALLINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, DECIMAL */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< unsupported feature, BINARY LARGE OBJECT */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DATE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITHOUT TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIME WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< TIMESTAMP WITH TIME ZONE */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        {
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */            
        },

        /**< DTL_TYPE_ROWID */
        {   
            dtlOperIsEqualRowIdToRowId,            /**< IS EQUAL */
            dtlOperIsNotEqualRowIdToRowId,         /**< IS NOT EQUAL */
            dtlOperIsLessThanRowIdToRowId,         /**< IS LESS THAN */
            dtlOperIsLessThanEqualRowIdToRowId,    /**< IS LESS THAN EQUAL */
            dtlOperIsGreaterThanRowIdToRowId,      /**< IS GREATER THAN */
            dtlOperIsGreaterThanEqualRowIdToRowId  /**< IS GREATER THAN EQUAL */
        }
        
    }
    
};

/*******************************************************************************
 * GET FUNCTION POINTER
 ******************************************************************************/

/* /\** */
/*  * @brief comparison operator의 함수 포인터 얻기 */
/*  * @param[in]  aOperator       comparison operation 종류 */
/*  * @param[in]  aLeftDataType   left operand의 data type */
/*  * @param[in]  aRightDataType  right operand의 data type */
/*  * @param[out] aFunctionPtr    함수 포인터 */
/*  * @param[out] aErrorStack     에러 스택 */
/*  *\/ */
/* stlStatus dtlGetCompareFunction( dtlComparisonOper    aOperator, */
/*                                  dtlDataType          aLeftDataType, */
/*                                  dtlDataType          aRightDataType, */
/*                                  dtlBuiltInFuncPtr  * aFunctionPtr, */
/*                                  stlErrorStack      * aErrorStack ) */
/* { */
/*     DTL_PARAM_VALIDATE( ( aOperator >= 0 ) && */
/*                         ( aOperator < DTL_COMP_OPER_MAX ), */
/*                         aErrorStack ); */

/*     DTL_PARAM_VALIDATE( ( aLeftDataType >= 0 ) && */
/*                         ( aLeftDataType < DTL_TYPE_MAX ), */
/*                         aErrorStack ); */

/*     if( dtlComparisonOperArgCount[ aOperator ] == 1 ) */
/*     { */
/*         *aFunctionPtr = dtlCompOperArg1FuncList[ aOperator - DTL_COMP_OPER_IS_NULL ]; */
/*     } */
/*     else */
/*     { */
/*         /\* 2-arguments comparison function *\/  */
/*         DTL_PARAM_VALIDATE( ( aRightDataType >= 0 ) && */
/*                             ( aRightDataType < DTL_TYPE_MAX ), */
/*                             aErrorStack ); */

/*         STL_TRY( dtlIsAvailableCompOper[ aLeftDataType ][ aRightDataType ] */
/*                  == STL_TRUE ); */

/*         *aFunctionPtr = dtlCompOperArg2FuncList[ aLeftDataType ][ aRightDataType ][ aOperator ]; */
/*     } */
    
/*     return STL_SUCCESS; */

/*     STL_FINISH; */
    
/*     return STL_FAILURE; */
/* } */

/* /\** */
/*  * @brief comparison operator의 함수 포인터 얻기 */
/*  * @param[in]  aOperator       comparison operation 종류 */
/*  * @param[in]  aLeftDataType   left operand의 data type */
/*  * @param[in]  aRightDataType  right operand의 data type */
/*  * @param[out] aFunctionID     함수 ID */
/*  * @param[out] aErrorStack     에러 스택 */
/*  * */
/*  * @todo Function Pointer 정보 리스트에서 Function의 ID를 얻어올 수 있어야 하며, */
/*  *  <BR> Function의 ID를 통해 Funtion Pointer를 찾을 수 있어야 한다. */
/*  *\/ */
/* stlStatus dtlGetCompareFunctionID( dtlComparisonOper   aOperator, */
/*                                    dtlDataType         aLeftDataType, */
/*                                    dtlDataType         aRightDataType, */
/*                                    stlUInt32         * aFunctionID, */
/*                                    stlErrorStack     * aErrorStack ) */
/* { */
/*     return STL_SUCCESS;     */
/* } */


/*******************************************************************************
 * COMMON 
 ******************************************************************************/

/**
 * @brief IsNull 연산 (dtlDataType 구분 없이 사용)
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNull 연산 결과
 * @param[in,out]  aInfo         Function 수행시 필요한 부가정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtlOperIsNull( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = ( aInputArgument->mValue.mDataValue->mLength == 0 );
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief IsNotNull 연산 (dtlDataType 구분 없이 사용)
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotNull 연산 결과
 * @param[in,out]  aInfo         Function 수행시 필요한 부가정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtlOperIsNotNull( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = ( aInputArgument->mValue.mDataValue->mLength != 0 );
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}


/*******************************************************************************
 * GET FUNCTION INFO 
 ******************************************************************************/

/**
 * @brief is null function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArrary의 갯수
 * @param[in]  aIsConstantData             constant value인지 여부
 * @param[in]  aDataTypeArray              function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray       aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[in]  aFuncArgDataTypeArrayCount  function의 input argument count
 * @param[out] aFuncArgDataTypeArray       function 수행시 수행 argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType         function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo  aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보
 * @param[out] aFuncPtrIdx                 함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoIsNull( stlUInt16               aDataTypeArrayCount,
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
                                stlErrorStack         * aErrorStack )
{
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 1, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 1, aErrorStack );

    
    /**
     * output 설정
     */
    
    aFuncArgDataTypeArray[0] = aDataTypeArray[0];

    aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
        gResultDataTypeDef[aFuncArgDataTypeArray[0]].mArgNum1;
    aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
        gResultDataTypeDef[aFuncArgDataTypeArray[0]].mArgNum2;
    aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
        gResultDataTypeDef[aFuncArgDataTypeArray[0]].mStringLengthUnit;
    aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
        gResultDataTypeDef[aFuncArgDataTypeArray[0]].mIntervalIndicator;
    
    *aFuncResultDataType = DTL_TYPE_BOOLEAN;

    aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mIntervalIndicator;
    
    *aFuncPtrIdx = 0;   /* dtlCompOperArg1FuncList */
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief is not null function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArrary의 갯수
 * @param[in]  aIsConstantData             constant value인지 여부
 * @param[in]  aDataTypeArray              function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray       aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[in]  aFuncArgDataTypeArrayCount  function의 input argument count
 * @param[out] aFuncArgDataTypeArray       function 수행시 수행 argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType         function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo  aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보
 * @param[out] aFuncPtrIdx                 함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoIsNotNull( stlUInt16               aDataTypeArrayCount,
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
                                   stlErrorStack         * aErrorStack )
{
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 1, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 1, aErrorStack );
    
    /**
     * output 설정
     */
    
    aFuncArgDataTypeArray[0] = aDataTypeArray[0];

    aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
        gResultDataTypeDef[aFuncArgDataTypeArray[0]].mArgNum1;
    aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
        gResultDataTypeDef[aFuncArgDataTypeArray[0]].mArgNum2;
    aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
        gResultDataTypeDef[aFuncArgDataTypeArray[0]].mStringLengthUnit;
    aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
        gResultDataTypeDef[aFuncArgDataTypeArray[0]].mIntervalIndicator;
    
    *aFuncResultDataType = DTL_TYPE_BOOLEAN;

    aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mIntervalIndicator;    
    
    *aFuncPtrIdx = 1;   /* dtlCompOperArg1FuncList */
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief is equal function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArrary의 갯수
 * @param[in]  aIsConstantData             constant value인지 여부
 * @param[in]  aDataTypeArray              function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray       aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[in]  aFuncArgDataTypeArrayCount  function의 input argument count
 * @param[out] aFuncArgDataTypeArray       function 수행시 수행 argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType         function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo  aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보
 * @param[out] aFuncPtrIdx                 함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoIsEqual( stlUInt16               aDataTypeArrayCount,
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
                                 stlErrorStack         * aErrorStack )
{
    const dtlCompareType    * sCompType;

    STL_PARAM_VALIDATE( aDataTypeArrayCount == 2, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 2, aErrorStack );

    /**
     * input data type에 conversion이 필요한 경우인지 확인
     */
    STL_TRY( dtlGetCompareConversionInfo( aIsConstantData[0],
                                          aIsConstantData[1],
                                          aDataTypeArray[0],
                                          aDataTypeArray[1],
                                          &sCompType,
                                          aErrorStack )
             == STL_SUCCESS );

    /**
     * function이 존재하는지 확인
     */
    
    STL_TRY_THROW( dtlCompOperArg2FuncList[ sCompType->mLeftType ][ sCompType->mRightType ][ DTL_COMP_OPER_IS_EQUAL ]
                   != NULL,
                   ERROR_NOT_APPLICABLE);

    
    /**
     * output 설정
     */

    if( aDataTypeArray[0] != sCompType->mLeftType )
    {
        aFuncArgDataTypeArray[0] = sCompType->mLeftType;

        aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
            gResultDataTypeDef[sCompType->mLeftType].mArgNum1;
        aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
            gResultDataTypeDef[sCompType->mLeftType].mArgNum2;
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
            gResultDataTypeDef[sCompType->mLeftType].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
            gResultDataTypeDef[sCompType->mLeftType].mIntervalIndicator;
    }
    else
    {
        aFuncArgDataTypeArray[0] = aDataTypeArray[0];

        aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
            aDataTypeDefInfoArray[0].mArgNum1;
        aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
            aDataTypeDefInfoArray[0].mArgNum2;
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
            aDataTypeDefInfoArray[0].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
            aDataTypeDefInfoArray[0].mIntervalIndicator;
    }

    if( aDataTypeArray[1] != sCompType->mRightType )
    {
        aFuncArgDataTypeArray[1] = sCompType->mRightType;

        aFuncArgDataTypeDefInfoArray[1].mArgNum1 =
            gResultDataTypeDef[sCompType->mRightType].mArgNum1;
        aFuncArgDataTypeDefInfoArray[1].mArgNum2 =
            gResultDataTypeDef[sCompType->mRightType].mArgNum2;
        aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit =
            gResultDataTypeDef[sCompType->mRightType].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
            gResultDataTypeDef[sCompType->mRightType].mIntervalIndicator;
    }
    else
    {
        aFuncArgDataTypeArray[1] = aDataTypeArray[1];

        aFuncArgDataTypeDefInfoArray[1].mArgNum1 =
            aDataTypeDefInfoArray[1].mArgNum1;
        aFuncArgDataTypeDefInfoArray[1].mArgNum2 =
            aDataTypeDefInfoArray[1].mArgNum2;
        aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit =
            aDataTypeDefInfoArray[1].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
            aDataTypeDefInfoArray[1].mIntervalIndicator;
    }


    /**
     * 함수의 argument 타입이 INTERVAL 타입인 경우,
     * conversion이 필요한 argument에 intervalIndicator 재설정.
     * 예) interval year = varchar 인 경우,
     *     varchar가 interval year로 변환될 수 있도록 intervalIndicator를 지정한다.
     */
    dtlSetCompareCastIntervalIndicator( aDataTypeArray,
                                        aDataTypeDefInfoArray,
                                        aFuncArgDataTypeArray,
                                        aFuncArgDataTypeDefInfoArray );

    *aFuncResultDataType = DTL_TYPE_BOOLEAN;

    aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mIntervalIndicator;

    /* dtlCompOperArg2FuncList */
    *aFuncPtrIdx = ( sCompType->mLeftType * 10000 ) + ( sCompType->mRightType * 100 ) + DTL_COMP_OPER_IS_EQUAL;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_COMPARISON_NOT_APPLICABLE,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[aDataTypeArray[0]],
                      dtlDataTypeName[aDataTypeArray[1]] );
    }
        
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief is not equal function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArrary의 갯수
 * @param[in]  aIsConstantData             constant value인지 여부
 * @param[in]  aDataTypeArray              function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray       aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[in]  aFuncArgDataTypeArrayCount  function의 input argument count
 * @param[out] aFuncArgDataTypeArray       function 수행시 수행 argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType         function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo  aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보
 * @param[out] aFuncPtrIdx                 함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoIsNotEqual( stlUInt16               aDataTypeArrayCount,
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
                                    stlErrorStack         * aErrorStack )
{
    const dtlCompareType    * sCompType;

    STL_PARAM_VALIDATE( aDataTypeArrayCount == 2, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 2, aErrorStack );

    /**
     * input data type에 conversion이 필요한 경우인지 확인
     */
    STL_TRY( dtlGetCompareConversionInfo( aIsConstantData[0],
                                          aIsConstantData[1],
                                          aDataTypeArray[0],
                                          aDataTypeArray[1],
                                          &sCompType,
                                          aErrorStack )
             == STL_SUCCESS );

    /**
     * function이 존재하는지 확인
     */
    
    STL_TRY_THROW( dtlCompOperArg2FuncList[ sCompType->mLeftType ][ sCompType->mRightType ][ DTL_COMP_OPER_IS_NOT_EQUAL ]
                   != NULL,
                   ERROR_NOT_APPLICABLE);

    
    /**
     * output 설정
     */

    if( aDataTypeArray[0] != sCompType->mLeftType )
    {
        aFuncArgDataTypeArray[0] = sCompType->mLeftType;

        aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
            gResultDataTypeDef[sCompType->mLeftType].mArgNum1;
        aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
            gResultDataTypeDef[sCompType->mLeftType].mArgNum2;
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
            gResultDataTypeDef[sCompType->mLeftType].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
            gResultDataTypeDef[sCompType->mLeftType].mIntervalIndicator;
    }
    else
    {
        aFuncArgDataTypeArray[0] = aDataTypeArray[0];

        aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
            aDataTypeDefInfoArray[0].mArgNum1;
        aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
            aDataTypeDefInfoArray[0].mArgNum2;
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
            aDataTypeDefInfoArray[0].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
            aDataTypeDefInfoArray[0].mIntervalIndicator;
    }

    if( aDataTypeArray[1] != sCompType->mRightType )
    {
        aFuncArgDataTypeArray[1] = sCompType->mRightType;

        aFuncArgDataTypeDefInfoArray[1].mArgNum1 =
            gResultDataTypeDef[sCompType->mRightType].mArgNum1;
        aFuncArgDataTypeDefInfoArray[1].mArgNum2 =
            gResultDataTypeDef[sCompType->mRightType].mArgNum2;
        aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit =
            gResultDataTypeDef[sCompType->mRightType].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
            gResultDataTypeDef[sCompType->mRightType].mIntervalIndicator;
    }
    else
    {
        aFuncArgDataTypeArray[1] = aDataTypeArray[1];

        aFuncArgDataTypeDefInfoArray[1].mArgNum1 =
            aDataTypeDefInfoArray[1].mArgNum1;
        aFuncArgDataTypeDefInfoArray[1].mArgNum2 =
            aDataTypeDefInfoArray[1].mArgNum2;
        aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit =
            aDataTypeDefInfoArray[1].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
            aDataTypeDefInfoArray[1].mIntervalIndicator;
    }

    /**
     * 함수의 argument 타입이 INTERVAL 타입인 경우,
     * conversion이 필요한 argument에 intervalIndicator 재설정.
     * 예) interval year = varchar 인 경우,
     *     varchar가 interval year로 변환될 수 있도록 intervalIndicator를 지정한다.
     */
    dtlSetCompareCastIntervalIndicator( aDataTypeArray,
                                        aDataTypeDefInfoArray,
                                        aFuncArgDataTypeArray,
                                        aFuncArgDataTypeDefInfoArray );

    *aFuncResultDataType = DTL_TYPE_BOOLEAN;

    aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mIntervalIndicator;

    /* dtlCompOperArg2FuncList */
    *aFuncPtrIdx = ( sCompType->mLeftType * 10000 ) + ( sCompType->mRightType * 100 ) +
        DTL_COMP_OPER_IS_NOT_EQUAL;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_COMPARISON_NOT_APPLICABLE,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[aDataTypeArray[0]],
                      dtlDataTypeName[aDataTypeArray[1]] );
    }
        
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief is less than function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArrary의 갯수
 * @param[in]  aIsConstantData             constant value인지 여부
 * @param[in]  aDataTypeArray              function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray       aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[in]  aFuncArgDataTypeArrayCount  function의 input argument count
 * @param[out] aFuncArgDataTypeArray       function 수행시 수행 argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType         function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo  aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보
 * @param[out] aFuncPtrIdx                 함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoIsLessThan( stlUInt16               aDataTypeArrayCount,
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
                                    stlErrorStack         * aErrorStack )
{
    const dtlCompareType    * sCompType;

    STL_PARAM_VALIDATE( aDataTypeArrayCount == 2, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 2, aErrorStack );

    /**
     * input data type에 conversion이 필요한 경우인지 확인
     */
    STL_TRY( dtlGetCompareConversionInfo( aIsConstantData[0],
                                          aIsConstantData[1],
                                          aDataTypeArray[0],
                                          aDataTypeArray[1],
                                          &sCompType,
                                          aErrorStack )
             == STL_SUCCESS );

    /**
     * function이 존재하는지 확인
     */
    
    STL_TRY_THROW( dtlCompOperArg2FuncList[ sCompType->mLeftType ][ sCompType->mRightType ][ DTL_COMP_OPER_IS_LESS_THAN ]
                   != NULL,
                   ERROR_NOT_APPLICABLE);

    
    /**
     * output 설정
     */

    if( aDataTypeArray[0] != sCompType->mLeftType )
    {
        aFuncArgDataTypeArray[0] = sCompType->mLeftType;

        aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
            gResultDataTypeDef[sCompType->mLeftType].mArgNum1;
        aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
            gResultDataTypeDef[sCompType->mLeftType].mArgNum2;
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
            gResultDataTypeDef[sCompType->mLeftType].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
            gResultDataTypeDef[sCompType->mLeftType].mIntervalIndicator;
    }
    else
    {
        aFuncArgDataTypeArray[0] = aDataTypeArray[0];

        aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
            aDataTypeDefInfoArray[0].mArgNum1;
        aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
            aDataTypeDefInfoArray[0].mArgNum2;
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
            aDataTypeDefInfoArray[0].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
            aDataTypeDefInfoArray[0].mIntervalIndicator;
    }

    if( aDataTypeArray[1] != sCompType->mRightType )
    {
        aFuncArgDataTypeArray[1] = sCompType->mRightType;

        aFuncArgDataTypeDefInfoArray[1].mArgNum1 =
            gResultDataTypeDef[sCompType->mRightType].mArgNum1;
        aFuncArgDataTypeDefInfoArray[1].mArgNum2 =
            gResultDataTypeDef[sCompType->mRightType].mArgNum2;
        aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit =
            gResultDataTypeDef[sCompType->mRightType].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
            gResultDataTypeDef[sCompType->mRightType].mIntervalIndicator;
    }
    else
    {
        aFuncArgDataTypeArray[1] = aDataTypeArray[1];

        aFuncArgDataTypeDefInfoArray[1].mArgNum1 =
            aDataTypeDefInfoArray[1].mArgNum1;
        aFuncArgDataTypeDefInfoArray[1].mArgNum2 =
            aDataTypeDefInfoArray[1].mArgNum2;
        aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit =
            aDataTypeDefInfoArray[1].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
            aDataTypeDefInfoArray[1].mIntervalIndicator;
    }

    /**
     * 함수의 argument 타입이 INTERVAL 타입인 경우,
     * conversion이 필요한 argument에 intervalIndicator 재설정.
     * 예) interval year = varchar 인 경우,
     *     varchar가 interval year로 변환될 수 있도록 intervalIndicator를 지정한다.
     */
    dtlSetCompareCastIntervalIndicator( aDataTypeArray,
                                        aDataTypeDefInfoArray,
                                        aFuncArgDataTypeArray,
                                        aFuncArgDataTypeDefInfoArray );

    *aFuncResultDataType = DTL_TYPE_BOOLEAN;

    aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mIntervalIndicator;

    /* dtlCompOperArg2FuncList */
    *aFuncPtrIdx = ( sCompType->mLeftType * 10000 ) + ( sCompType->mRightType * 100 ) +
        DTL_COMP_OPER_IS_LESS_THAN;    
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_COMPARISON_NOT_APPLICABLE,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[aDataTypeArray[0]],
                      dtlDataTypeName[aDataTypeArray[1]] );
    }
        
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief is less than equal function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArrary의 갯수
 * @param[in]  aIsConstantData             constant value인지 여부
 * @param[in]  aDataTypeArray              function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray       aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[in]  aFuncArgDataTypeArrayCount  function의 input argument count
 * @param[out] aFuncArgDataTypeArray       function 수행시 수행 argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType         function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo  aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보
 * @param[out] aFuncPtrIdx                 함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoIsLessThanEqual( stlUInt16               aDataTypeArrayCount,
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
                                         stlErrorStack         * aErrorStack )
{
    const dtlCompareType    * sCompType;

    STL_PARAM_VALIDATE( aDataTypeArrayCount == 2, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 2, aErrorStack );

    /**
     * input data type에 conversion이 필요한 경우인지 확인
     */
    STL_TRY( dtlGetCompareConversionInfo( aIsConstantData[0],
                                          aIsConstantData[1],
                                          aDataTypeArray[0],
                                          aDataTypeArray[1],
                                          &sCompType,
                                          aErrorStack )
             == STL_SUCCESS );

    /**
     * function이 존재하는지 확인
     */
    
    STL_TRY_THROW( dtlCompOperArg2FuncList[ sCompType->mLeftType ][ sCompType->mRightType ][ DTL_COMP_OPER_IS_LESS_THAN_EQUAL ]
                   != NULL,
                   ERROR_NOT_APPLICABLE);

    
    /**
     * output 설정
     */

    if( aDataTypeArray[0] != sCompType->mLeftType )
    {
        aFuncArgDataTypeArray[0] = sCompType->mLeftType;

        aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
            gResultDataTypeDef[sCompType->mLeftType].mArgNum1;
        aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
            gResultDataTypeDef[sCompType->mLeftType].mArgNum2;
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
            gResultDataTypeDef[sCompType->mLeftType].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
            gResultDataTypeDef[sCompType->mLeftType].mIntervalIndicator;
    }
    else
    {
        aFuncArgDataTypeArray[0] = aDataTypeArray[0];

        aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
            aDataTypeDefInfoArray[0].mArgNum1;
        aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
            aDataTypeDefInfoArray[0].mArgNum2;
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
            aDataTypeDefInfoArray[0].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
            aDataTypeDefInfoArray[0].mIntervalIndicator;
    }

    if( aDataTypeArray[1] != sCompType->mRightType )
    {
        aFuncArgDataTypeArray[1] = sCompType->mRightType;

        aFuncArgDataTypeDefInfoArray[1].mArgNum1 =
            gResultDataTypeDef[sCompType->mRightType].mArgNum1;
        aFuncArgDataTypeDefInfoArray[1].mArgNum2 =
            gResultDataTypeDef[sCompType->mRightType].mArgNum2;
        aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit =
            gResultDataTypeDef[sCompType->mRightType].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
            gResultDataTypeDef[sCompType->mRightType].mIntervalIndicator;
    }
    else
    {
        aFuncArgDataTypeArray[1] = aDataTypeArray[1];

        aFuncArgDataTypeDefInfoArray[1].mArgNum1 =
            aDataTypeDefInfoArray[1].mArgNum1;
        aFuncArgDataTypeDefInfoArray[1].mArgNum2 =
            aDataTypeDefInfoArray[1].mArgNum2;
        aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit =
            aDataTypeDefInfoArray[1].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
            aDataTypeDefInfoArray[1].mIntervalIndicator;
    }

    /**
     * 함수의 argument 타입이 INTERVAL 타입인 경우,
     * conversion이 필요한 argument에 intervalIndicator 재설정.
     * 예) interval year = varchar 인 경우,
     *     varchar가 interval year로 변환될 수 있도록 intervalIndicator를 지정한다.
     */
    dtlSetCompareCastIntervalIndicator( aDataTypeArray,
                                        aDataTypeDefInfoArray,
                                        aFuncArgDataTypeArray,
                                        aFuncArgDataTypeDefInfoArray );

    *aFuncResultDataType = DTL_TYPE_BOOLEAN;

    aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mIntervalIndicator;

    /* dtlCompOperArg2FuncList */
    *aFuncPtrIdx = ( sCompType->mLeftType * 10000 ) + ( sCompType->mRightType * 100 ) +
        DTL_COMP_OPER_IS_LESS_THAN_EQUAL;        
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_COMPARISON_NOT_APPLICABLE,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[aDataTypeArray[0]],
                      dtlDataTypeName[aDataTypeArray[1]] );
    }
        
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief is greater than function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArrary의 갯수
 * @param[in]  aIsConstantData             constant value인지 여부
 * @param[in]  aDataTypeArray              function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray       aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[in]  aFuncArgDataTypeArrayCount  function의 input argument count
 * @param[out] aFuncArgDataTypeArray       function 수행시 수행 argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType         function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo  aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보
 * @param[out] aFuncPtrIdx                 함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoIsGreaterThan( stlUInt16               aDataTypeArrayCount,
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
                                       stlErrorStack         * aErrorStack )
{
    const dtlCompareType    * sCompType;

    STL_PARAM_VALIDATE( aDataTypeArrayCount == 2, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 2, aErrorStack );

    /**
     * input data type에 conversion이 필요한 경우인지 확인
     */
    STL_TRY( dtlGetCompareConversionInfo( aIsConstantData[0],
                                          aIsConstantData[1],
                                          aDataTypeArray[0],
                                          aDataTypeArray[1],
                                          &sCompType,
                                          aErrorStack )
             == STL_SUCCESS );

    /**
     * function이 존재하는지 확인
     */
    
    STL_TRY_THROW( dtlCompOperArg2FuncList[ sCompType->mLeftType ][ sCompType->mRightType ][ DTL_COMP_OPER_IS_GREATER_THAN ]
                   != NULL,
                   ERROR_NOT_APPLICABLE);

    
    /**
     * output 설정
     */

    if( aDataTypeArray[0] != sCompType->mLeftType )
    {
        aFuncArgDataTypeArray[0] = sCompType->mLeftType;

        aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
            gResultDataTypeDef[sCompType->mLeftType].mArgNum1;
        aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
            gResultDataTypeDef[sCompType->mLeftType].mArgNum2;
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
            gResultDataTypeDef[sCompType->mLeftType].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
            gResultDataTypeDef[sCompType->mLeftType].mIntervalIndicator;
    }
    else
    {
        aFuncArgDataTypeArray[0] = aDataTypeArray[0];

        aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
            aDataTypeDefInfoArray[0].mArgNum1;
        aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
            aDataTypeDefInfoArray[0].mArgNum2;
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
            aDataTypeDefInfoArray[0].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
            aDataTypeDefInfoArray[0].mIntervalIndicator;
    }

    if( aDataTypeArray[1] != sCompType->mRightType )
    {
        aFuncArgDataTypeArray[1] = sCompType->mRightType;

        aFuncArgDataTypeDefInfoArray[1].mArgNum1 =
            gResultDataTypeDef[sCompType->mRightType].mArgNum1;
        aFuncArgDataTypeDefInfoArray[1].mArgNum2 =
            gResultDataTypeDef[sCompType->mRightType].mArgNum2;
        aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit =
            gResultDataTypeDef[sCompType->mRightType].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
            gResultDataTypeDef[sCompType->mRightType].mIntervalIndicator;
    }
    else
    {
        aFuncArgDataTypeArray[1] = aDataTypeArray[1];

        aFuncArgDataTypeDefInfoArray[1].mArgNum1 =
            aDataTypeDefInfoArray[1].mArgNum1;
        aFuncArgDataTypeDefInfoArray[1].mArgNum2 =
            aDataTypeDefInfoArray[1].mArgNum2;
        aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit =
            aDataTypeDefInfoArray[1].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
            aDataTypeDefInfoArray[1].mIntervalIndicator;
    }

    /**
     * 함수의 argument 타입이 INTERVAL 타입인 경우,
     * conversion이 필요한 argument에 intervalIndicator 재설정.
     * 예) interval year = varchar 인 경우,
     *     varchar가 interval year로 변환될 수 있도록 intervalIndicator를 지정한다.
     */
    dtlSetCompareCastIntervalIndicator( aDataTypeArray,
                                        aDataTypeDefInfoArray,
                                        aFuncArgDataTypeArray,
                                        aFuncArgDataTypeDefInfoArray );

    *aFuncResultDataType = DTL_TYPE_BOOLEAN;

    aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mIntervalIndicator;

    /* dtlCompOperArg2FuncList */
    *aFuncPtrIdx = ( sCompType->mLeftType * 10000 ) + ( sCompType->mRightType * 100 ) +
        DTL_COMP_OPER_IS_GREATER_THAN;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_COMPARISON_NOT_APPLICABLE,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[aDataTypeArray[0]],
                      dtlDataTypeName[aDataTypeArray[1]] );
    }
        
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief is greater than equal function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArrary의 갯수
 * @param[in]  aIsConstantData             constant value인지 여부
 * @param[in]  aDataTypeArray              function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray       aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[in]  aFuncArgDataTypeArrayCount  function의 input argument count
 * @param[out] aFuncArgDataTypeArray       function 수행시 수행 argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType         function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo  aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보
 * @param[out] aFuncPtrIdx                 함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoIsGreaterThanEqual( stlUInt16               aDataTypeArrayCount,
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
                                            stlErrorStack         * aErrorStack )
{
    const dtlCompareType    * sCompType;

    STL_PARAM_VALIDATE( aDataTypeArrayCount == 2, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 2, aErrorStack );

    /**
     * input data type에 conversion이 필요한 경우인지 확인
     */
    STL_TRY( dtlGetCompareConversionInfo( aIsConstantData[0],
                                          aIsConstantData[1],
                                          aDataTypeArray[0],
                                          aDataTypeArray[1],
                                          &sCompType,
                                          aErrorStack )
             == STL_SUCCESS );

    /**
     * function이 존재하는지 확인
     */
    
    STL_TRY_THROW( dtlCompOperArg2FuncList[ sCompType->mLeftType ][ sCompType->mRightType ][ DTL_COMP_OPER_IS_GREATER_THAN_EQUAL ]
                   != NULL,
                   ERROR_NOT_APPLICABLE);

    
    /**
     * output 설정
     */

    if( aDataTypeArray[0] != sCompType->mLeftType )
    {
        aFuncArgDataTypeArray[0] = sCompType->mLeftType;

        aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
            gResultDataTypeDef[sCompType->mLeftType].mArgNum1;
        aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
            gResultDataTypeDef[sCompType->mLeftType].mArgNum2;
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
            gResultDataTypeDef[sCompType->mLeftType].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
            gResultDataTypeDef[sCompType->mLeftType].mIntervalIndicator;
    }
    else
    {
        aFuncArgDataTypeArray[0] = aDataTypeArray[0];

        aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
            aDataTypeDefInfoArray[0].mArgNum1;
        aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
            aDataTypeDefInfoArray[0].mArgNum2;
        aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
            aDataTypeDefInfoArray[0].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
            aDataTypeDefInfoArray[0].mIntervalIndicator;
    }

    if( aDataTypeArray[1] != sCompType->mRightType )
    {
        aFuncArgDataTypeArray[1] = sCompType->mRightType;

        aFuncArgDataTypeDefInfoArray[1].mArgNum1 =
            gResultDataTypeDef[sCompType->mRightType].mArgNum1;
        aFuncArgDataTypeDefInfoArray[1].mArgNum2 =
            gResultDataTypeDef[sCompType->mRightType].mArgNum2;
        aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit =
            gResultDataTypeDef[sCompType->mRightType].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
            gResultDataTypeDef[sCompType->mRightType].mIntervalIndicator;
    }
    else
    {
        aFuncArgDataTypeArray[1] = aDataTypeArray[1];

        aFuncArgDataTypeDefInfoArray[1].mArgNum1 =
            aDataTypeDefInfoArray[1].mArgNum1;
        aFuncArgDataTypeDefInfoArray[1].mArgNum2 =
            aDataTypeDefInfoArray[1].mArgNum2;
        aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit =
            aDataTypeDefInfoArray[1].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
            aDataTypeDefInfoArray[1].mIntervalIndicator;
    }

    /**
     * 함수의 argument 타입이 INTERVAL 타입인 경우,
     * conversion이 필요한 argument에 intervalIndicator 재설정.
     * 예) interval year = varchar 인 경우,
     *     varchar가 interval year로 변환될 수 있도록 intervalIndicator를 지정한다.
     */
    dtlSetCompareCastIntervalIndicator( aDataTypeArray,
                                        aDataTypeDefInfoArray,
                                        aFuncArgDataTypeArray,
                                        aFuncArgDataTypeDefInfoArray );
    
    *aFuncResultDataType = DTL_TYPE_BOOLEAN;

    aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mIntervalIndicator;
    
    /* dtlCompOperArg2FuncList */
    *aFuncPtrIdx = ( sCompType->mLeftType * 10000 ) + ( sCompType->mRightType * 100 ) +
        DTL_COMP_OPER_IS_GREATER_THAN_EQUAL;    
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_COMPARISON_NOT_APPLICABLE,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[aDataTypeArray[0]],
                      dtlDataTypeName[aDataTypeArray[1]] );
    }
        
    STL_FINISH;
    
    return STL_FAILURE;
}


/*******************************************************************************
 * GET FUNCTION POINTER 
 ******************************************************************************/

/**
 * @brief is null function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrIsNull( stlUInt32             aFuncPtrIdx,
                               dtlBuiltInFuncPtr   * aFuncPtr,
                               stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );

    /**
     * output 설정
     */
    
    *aFuncPtr = dtlCompOperArg1FuncList[ aFuncPtrIdx ];
    
    return STL_SUCCESS;
        
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief is not null function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrIsNotNull( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 1, aErrorStack );

    /**
     * output 설정
     */
    
    *aFuncPtr = dtlCompOperArg1FuncList[ aFuncPtrIdx ];
    
    return STL_SUCCESS;
        
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief is equal function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrIsEqual( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack )
{
    stlUInt8  sType1;
    stlUInt8  sType2;
    stlUInt8  sOperType;
    
    /**
     * output 설정
     */
    
    sType1 = aFuncPtrIdx / 10000;
    sType2 = ( aFuncPtrIdx - sType1 * 10000 ) / 100;
    sOperType = aFuncPtrIdx % 100;

    *aFuncPtr = dtlCompOperArg2FuncList[ sType1 ][ sType2 ][ sOperType ];
    
    return STL_SUCCESS;
}


/**
 * @brief is not equal function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrIsNotEqual( stlUInt32             aFuncPtrIdx,
                                   dtlBuiltInFuncPtr   * aFuncPtr,
                                   stlErrorStack       * aErrorStack )
{
    stlUInt8  sType1;
    stlUInt8  sType2;
    stlUInt8  sOperType;
    
    /**
     * output 설정
     */
    
    sType1 = aFuncPtrIdx / 10000;
    sType2 = ( aFuncPtrIdx - sType1 * 10000 ) / 100;
    sOperType = aFuncPtrIdx % 100;

    *aFuncPtr = dtlCompOperArg2FuncList[ sType1 ][ sType2 ][ sOperType ];
    
    return STL_SUCCESS;
}


/**
 * @brief is less than function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrIsLessThan( stlUInt32             aFuncPtrIdx,
                                   dtlBuiltInFuncPtr   * aFuncPtr,
                                   stlErrorStack       * aErrorStack )
{
    stlUInt8  sType1;
    stlUInt8  sType2;
    stlUInt8  sOperType;
    
    /**
     * output 설정
     */
    
    sType1 = aFuncPtrIdx / 10000;
    sType2 = ( aFuncPtrIdx - sType1 * 10000 ) / 100;
    sOperType = aFuncPtrIdx % 100;

    *aFuncPtr = dtlCompOperArg2FuncList[ sType1 ][ sType2 ][ sOperType ];
    
    return STL_SUCCESS;
}


/**
 * @brief is less than equal function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrIsLessThanEqual( stlUInt32             aFuncPtrIdx,
                                        dtlBuiltInFuncPtr   * aFuncPtr,
                                        stlErrorStack       * aErrorStack )
{
    stlUInt8  sType1;
    stlUInt8  sType2;
    stlUInt8  sOperType;
    
    /**
     * output 설정
     */
    
    sType1 = aFuncPtrIdx / 10000;
    sType2 = ( aFuncPtrIdx - sType1 * 10000 ) / 100;
    sOperType = aFuncPtrIdx % 100;

    *aFuncPtr = dtlCompOperArg2FuncList[ sType1 ][ sType2 ][ sOperType ];
    
    return STL_SUCCESS;
}


/**
 * @brief is greater than function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrIsGreaterThan( stlUInt32             aFuncPtrIdx,
                                      dtlBuiltInFuncPtr   * aFuncPtr,
                                      stlErrorStack       * aErrorStack )
{
    stlUInt8  sType1;
    stlUInt8  sType2;
    stlUInt8  sOperType;
    
    /**
     * output 설정
     */
    
    sType1 = aFuncPtrIdx / 10000;
    sType2 = ( aFuncPtrIdx - sType1 * 10000 ) / 100;
    sOperType = aFuncPtrIdx % 100;

    *aFuncPtr = dtlCompOperArg2FuncList[ sType1 ][ sType2 ][ sOperType ];
    
    return STL_SUCCESS;
}


/**
 * @brief is greater than equal function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrIsGreaterThanEqual( stlUInt32             aFuncPtrIdx,
                                           dtlBuiltInFuncPtr   * aFuncPtr,
                                           stlErrorStack       * aErrorStack )
{
    stlUInt8  sType1;
    stlUInt8  sType2;
    stlUInt8  sOperType;
    
    /**
     * output 설정
     */
    
    sType1 = aFuncPtrIdx / 10000;
    sType2 = ( aFuncPtrIdx - sType1 * 10000 ) / 100;
    sOperType = aFuncPtrIdx % 100;

    *aFuncPtr = dtlCompOperArg2FuncList[ sType1 ][ sType2 ][ sOperType ];
    
    return STL_SUCCESS;
}

/**
 * @brief compare column간의 conversion정보를 얻는다.
 *   <BR> (dtdConversionGroup matrix와 dtlCompareFuncList matrix 참고)
 *   <BR> 아래와 같은 질의의 경우, 비교를 위해서 conversion이 수행되어야 한다.
 *   <BR>   예)   WHERE I1(VARCHAR) = I2(BIGINT)
 *   <BR>    ==>  WHERE I1(NUMBER) = I2(NUMBER)
 *   
 * @param[in]  aIsLeftConstantData  left value가 constant인지 여부
 * @param[in]  aIsRightConstantData right value가 constant인지 여부
 * @param[in]  aLeftInputDataType   left input data type
 * @param[in]  aRightInputDataType  right input data type
 * @param[out] aCompareType         실제 function의 argument로 수행될 datatype
 * @param[out] aErrorStack            에러스택 
 */
stlStatus dtlGetCompareConversionInfo( stlBool                    aIsLeftConstantData,
                                       stlBool                    aIsRightConstantData,
                                       dtlDataType                aLeftInputDataType,
                                       dtlDataType                aRightInputDataType,
                                       const dtlCompareType    ** aCompareType,
                                       stlErrorStack            * aErrorStack )
{
    STL_PARAM_VALIDATE( ( aLeftInputDataType >= DTL_TYPE_BOOLEAN ) &&
                        ( aLeftInputDataType < DTL_TYPE_MAX ),
                        aErrorStack );
    STL_PARAM_VALIDATE( ( aRightInputDataType >= DTL_TYPE_BOOLEAN ) &&
                        ( aRightInputDataType < DTL_TYPE_MAX ),
                        aErrorStack );

    *aCompareType = & dtlCompOperArgType[ aLeftInputDataType ][ aRightInputDataType ]
        [ (stlInt32) aIsLeftConstantData ][ (stlInt32) aIsRightConstantData ];

    STL_TRY_THROW( ( (*aCompareType)->mLeftType != DTL_TYPE_NA ) &&
                   ( (*aCompareType)->mRightType != DTL_TYPE_NA ),
                   ERROR_NOT_APPLICABLE );


    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_COMPARISON_NOT_APPLICABLE,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[ aLeftInputDataType ],
                      dtlDataTypeName[ aRightInputDataType ] );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief interval type으로 cast된 경우, interval indicator를 설정한다.
 * @param[in]  aDataTypeArray                  input argument datatype 배열
 * @param[in]  aDataTypeDefInfoArray           input arguemtn type info
 * @param[in]  aFuncArgDataTypeArray           function argument datatype 배열
 * @param[out] aFuncArgDataTypeDefInfoArray    function argument type info
 *        
 */
void dtlSetCompareCastIntervalIndicator( dtlDataType           * aDataTypeArray,
                                         dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                         dtlDataType           * aFuncArgDataTypeArray,
                                         dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray )
{
    /**
     * 함수의 argument 타입이 INTERVAL 타입인 경우,
     * conversion이 필요한 argument에 intervalIndicator 재설정.
     * 예) interval year = varchar 인 경우,
     *     varchar가 interval year로 변환될 수 있도록 intervalIndicator를 지정한다.
     */
    if( ((aFuncArgDataTypeArray[0] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) &&
         (aFuncArgDataTypeArray[1] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH))
        ||
        ((aFuncArgDataTypeArray[0] == DTL_TYPE_INTERVAL_DAY_TO_SECOND) &&
         (aFuncArgDataTypeArray[1] == DTL_TYPE_INTERVAL_DAY_TO_SECOND)) )
    {
        if( ((aDataTypeArray[0] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) &&
             (aDataTypeArray[1] != DTL_TYPE_INTERVAL_YEAR_TO_MONTH))
             ||
            ((aDataTypeArray[0] == DTL_TYPE_INTERVAL_DAY_TO_SECOND) &&
             (aDataTypeArray[1] != DTL_TYPE_INTERVAL_DAY_TO_SECOND)) )
        {
            aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
                aDataTypeDefInfoArray[0].mIntervalIndicator;
            aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
                aDataTypeDefInfoArray[0].mIntervalIndicator;
        }
        else if( ((aDataTypeArray[1] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) &&
                  (aDataTypeArray[0] != DTL_TYPE_INTERVAL_YEAR_TO_MONTH))
                 ||
                 ((aDataTypeArray[1] == DTL_TYPE_INTERVAL_DAY_TO_SECOND) &&
                  (aDataTypeArray[0] != DTL_TYPE_INTERVAL_DAY_TO_SECOND)) )
        {
            aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
                aDataTypeDefInfoArray[1].mIntervalIndicator;
            aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
                aDataTypeDefInfoArray[1].mIntervalIndicator;
        }
        else
        {
            // Do Nothing
        }
    }
    else
    {
        // Do Nothing
    }    
}

/** @} dtlOperCompare */
