/*******************************************************************************
 * dtlPhysicalFunc.c
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
 * @file dtlPhysicalFunc.c
 * @brief Align이 보장되지 않은 value간의 연산
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfNumeric.h>


/**
 * @addtogroup dtlPhysicalFunc
 * @{
 */


/*******************************************************************************
 * 1-Argument Column FUNCTION
 ******************************************************************************/

/**
 * @brief IS TRUE 평가 연산
 *   <BR> Boolean type 만 올 수 있다.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsTrue( void       * aLeftVal,
                               stlInt64     aLeftLen,
                               void       * aRightVal,
                               stlInt64     aRightLen )
{
    return ( aLeftLen == DTL_BOOLEAN_SIZE ) && ( ((stlChar*) aLeftVal)[0] == STL_TRUE );
}


/**
 * @brief IS FALSE 평가 연산
 *   <BR> Boolean type 만 올 수 있다.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsFalse( void       * aLeftVal,
                                stlInt64     aLeftLen,
                                void       * aRightVal,
                                stlInt64     aRightLen )
{
    return ( aLeftLen == DTL_BOOLEAN_SIZE ) && ( ((stlChar*) aLeftVal)[0] == STL_FALSE );
}


/*******************************************************************************
 * 1-Argument FUNCTIONS
 ******************************************************************************/


/**
 * @brief IS 연산 
 *   <BR> Boolean type 만이 IS 연산의 operand가 될 수 있다.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIs( void       * aLeftVal,
                           stlInt64     aLeftLen,
                           void       * aRightVal,
                           stlInt64     aRightLen )
{
    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_BOOLEAN_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_BOOLEAN_SIZE ) ); 

    if( aLeftLen == aRightLen )
    {
        return ((stlChar*) aLeftVal)[0] == ((stlChar*) aRightVal)[0];
    }
    else
    {
        return STL_FALSE;
    }
}


/**
 * @brief IS NOT 연산 
 *   <BR> Boolean type 만이 IS NOT 연산의 operand가 될 수 있다.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
stlBool dtlPhysicalFuncIsNot( void       * aLeftVal,
                              stlInt64     aLeftLen,
                              void       * aRightVal,
                              stlInt64     aRightLen )
{
    STL_DASSERT( ( aLeftLen == 0 ) || ( aLeftLen == DTL_BOOLEAN_SIZE ) ); 
    STL_DASSERT( ( aRightLen == 0 ) || ( aRightLen == DTL_BOOLEAN_SIZE ) ); 

    if( aLeftLen == aRightLen )
    {
        return ((stlChar*) aLeftVal)[0] != ((stlChar*) aRightVal)[0];
    }
    else
    {
        return STL_TRUE;
    }
}


/**
 * @brief IS NULL 연산 
 *   <BR> 모든 type이 IS NULL 연산의 operand가 될 수 있다.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 *
 * @remark right value는 사용되지 않음
 */
stlBool dtlPhysicalFuncIsNull( void       * aLeftVal,
                               stlInt64     aLeftLen,
                               void       * aRightVal,
                               stlInt64     aRightLen )
{
    return ( aLeftLen == 0 );
}


/**
 * @brief IS NOT NULL 연산 
 *   <BR> 모든 type이 IS NULL 연산의 operand가 될 수 있다.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 *
 * @remark right value는 사용되지 않음
 */
stlBool dtlPhysicalFuncIsNotNull( void       * aLeftVal,
                                  stlInt64     aLeftLen,
                                  void       * aRightVal,
                                  stlInt64     aRightLen )
{
    return ( aLeftLen != 0 );
}



/*******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

const dtlPhysicalFunc dtlPhysicalFuncArg1FuncList[ DTL_PHYSICAL_FUNC_MAX - DTL_PHYSICAL_FUNC_IS ] =
{
    dtlPhysicalFuncIs,              /**< IS */
    dtlPhysicalFuncIsNot,           /**< IS NOT */
    dtlPhysicalFuncIsNull,          /**< IS NULL */
    dtlPhysicalFuncIsNotNull,       /**< IS NOT NULL */
    dtlPhysicalFuncIsFalse          /**< NOT */
};

/**
 * @todo IN Physical Function Pointer
 * aIsInPhysical = STL_TRUE인 경우, 기존의 Physical Function 과 달리
 * Left와 Right의 void형 포인트로 dtlDataValue 가 list 로 오게된다.
 * Function pointer를 통해 dtlDataValue를 physical function안에서 하나씩 처리하도록 함.
 */
const dtlPhysicalFunc dtlInPhysicalFuncPointer[DTL_IN_PHYSICAL_FILTER_CNT] =
{
    /**
     * IN
     */
    
    dtlInPhysicalFuncListToList,                 /**< IN */
    dtlNotInPhysicalFuncListToList,              /**< NOT IN */

    /**
     * EXISTS
     */
    
    NULL,              /**< EXISTS */
    NULL,              /**< NOT EXISTS */

    /**
     * LIST OPERATION ( ANY )
     */
    dtlInPhysicalFuncListToList,                  /**< EQUAL ANY */
    dtlNotEqualAnyPhysicalFunc,                   /**< NOT EQUAL ANY */
    dtlLessThanAnyPhysicalFunc,                   /**< LESS THANANY */ 
    dtlLessThanEqualAnyPhysicalFunc,              /**< LESS THAN EQUAL ANY */
    dtlGreaterThanAnyPhysicalFunc,                /**< GREATER THAN ANY */
    dtlGreaterThanEqualAnyPhysicalFunc,           /**< GREATER THAN EQUAL ANY */

    /**
     * LIST OPERATION ( ALL )
     */
    dtlEqualAllPhysicalFunc,                      /**< EQUAL ALL */
    dtlNotInPhysicalFuncListToList,               /**< NOT EQUAL ALL */
    dtlLessThanAllPhysicalFunc,                   /**< LESS THANALL */ 
    dtlLessThanEqualAllPhysicalFunc,              /**< LESS THAN EQUAL ALL */
    dtlGreaterThanAllPhysicalFunc,                /**< GREATER THAN ALL */
    dtlGreaterThanEqualAllPhysicalFunc            /**< GREATER THAN EQUAL ALL */
};  

const dtlPhysicalFunc dtlPhysicalFuncArg2FuncList[ DTL_TYPE_MAX ][ DTL_TYPE_MAX ][ DTL_PHYSICAL_FUNC_IS ] =
{
    /*******************************************************************************
     * BOOLEAN
     ******************************************************************************/
    /**< BOOLEAN */
    {
    
        /**< BOOLEAN */
        {   
            dtlPhysicalFuncIsEqualBooleanToBoolean,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualBooleanToBoolean,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanBooleanToBoolean,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualBooleanToBoolean,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanBooleanToBoolean,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualBooleanToBoolean /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualSmallIntToSmallInt,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualSmallIntToSmallInt,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanSmallIntToSmallInt,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualSmallIntToSmallInt,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanSmallIntToSmallInt,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualSmallIntToSmallInt /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlPhysicalFuncIsEqualSmallIntToInteger,            /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualSmallIntToInteger,         /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanSmallIntToInteger,         /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualSmallIntToInteger,    /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanSmallIntToInteger,      /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualSmallIntToInteger  /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlPhysicalFuncIsEqualSmallIntToBigInt,             /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualSmallIntToBigInt,          /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanSmallIntToBigInt,          /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualSmallIntToBigInt,     /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanSmallIntToBigInt,       /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualSmallIntToBigInt   /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            dtlPhysicalFuncIsEqualSmallIntToReal,               /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualSmallIntToReal,            /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanSmallIntToReal,            /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualSmallIntToReal,       /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanSmallIntToReal,         /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualSmallIntToReal     /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlPhysicalFuncIsEqualSmallIntToDouble,             /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualSmallIntToDouble,          /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanSmallIntToDouble,          /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualSmallIntToDouble,     /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanSmallIntToDouble,       /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualSmallIntToDouble   /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlPhysicalFuncIsEqualSmallIntToNumeric,                  /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualSmallIntToNumeric,               /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanSmallIntToNumeric,               /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualSmallIntToNumeric,          /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanSmallIntToNumeric,            /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualSmallIntToNumeric        /**< IS GREATER THAN EQUAL */
        },

        /**< NUMBER */
        {   
            dtlPhysicalFuncIsEqualSmallIntToNumeric,                  /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualSmallIntToNumeric,               /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanSmallIntToNumeric,               /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualSmallIntToNumeric,          /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanSmallIntToNumeric,            /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualSmallIntToNumeric        /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualIntegerToSmallInt,            /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualIntegerToSmallInt,         /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanIntegerToSmallInt,         /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualIntegerToSmallInt,    /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanIntegerToSmallInt,      /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualIntegerToSmallInt  /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlPhysicalFuncIsEqualIntegerToInteger,             /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualIntegerToInteger,          /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanIntegerToInteger,          /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualIntegerToInteger,     /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanIntegerToInteger,       /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualIntegerToInteger   /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlPhysicalFuncIsEqualIntegerToBigInt,              /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualIntegerToBigInt,           /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanIntegerToBigInt,           /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualIntegerToBigInt,      /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanIntegerToBigInt,        /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualIntegerToBigInt    /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            dtlPhysicalFuncIsEqualIntegerToReal,                /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualIntegerToReal,             /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanIntegerToReal,             /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualIntegerToReal,        /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanIntegerToReal,          /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualIntegerToReal      /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlPhysicalFuncIsEqualIntegerToDouble,              /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualIntegerToDouble,           /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanIntegerToDouble,           /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualIntegerToDouble,      /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanIntegerToDouble,        /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualIntegerToDouble    /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlPhysicalFuncIsEqualIntegerToNumeric,                /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualIntegerToNumeric,             /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanIntegerToNumeric,             /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualIntegerToNumeric,        /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanIntegerToNumeric,          /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualIntegerToNumeric      /**< IS GREATER THAN EQUAL */
        },

        /**< NUMBER */
        {   
            dtlPhysicalFuncIsEqualIntegerToNumeric,                /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualIntegerToNumeric,             /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanIntegerToNumeric,             /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualIntegerToNumeric,        /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanIntegerToNumeric,          /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualIntegerToNumeric      /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualBigIntToSmallInt,             /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualBigIntToSmallInt,          /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanBigIntToSmallInt,          /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualBigIntToSmallInt,     /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanBigIntToSmallInt,       /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualBigIntToSmallInt   /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlPhysicalFuncIsEqualBigIntToInteger,              /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualBigIntToInteger,           /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanBigIntToInteger,           /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualBigIntToInteger,      /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanBigIntToInteger,        /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualBigIntToInteger    /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlPhysicalFuncIsEqualBigIntToBigInt,               /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualBigIntToBigInt,            /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanBigIntToBigInt,            /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualBigIntToBigInt,       /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanBigIntToBigInt,         /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualBigIntToBigInt     /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            dtlPhysicalFuncIsEqualBigIntToReal,                 /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualBigIntToReal,              /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanBigIntToReal,              /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualBigIntToReal,         /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanBigIntToReal,           /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualBigIntToReal       /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlPhysicalFuncIsEqualBigIntToDouble,               /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualBigIntToDouble,            /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanBigIntToDouble,            /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualBigIntToDouble,       /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanBigIntToDouble,         /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualBigIntToDouble     /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlPhysicalFuncIsEqualBigIntToNumeric,               /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualBigIntToNumeric,            /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanBigIntToNumeric,            /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualBigIntToNumeric,       /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanBigIntToNumeric,         /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualBigIntToNumeric     /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            dtlPhysicalFuncIsEqualBigIntToNumeric,               /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualBigIntToNumeric,            /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanBigIntToNumeric,            /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualBigIntToNumeric,       /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanBigIntToNumeric,         /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualBigIntToNumeric     /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualRealToSmallInt,               /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualRealToSmallInt,            /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanRealToSmallInt,            /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualRealToSmallInt,       /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanRealToSmallInt,         /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualRealToSmallInt     /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlPhysicalFuncIsEqualRealToInteger,                /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualRealToInteger,             /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanRealToInteger,             /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualRealToInteger,        /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanRealToInteger,          /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualRealToInteger      /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlPhysicalFuncIsEqualRealToBigInt,                 /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualRealToBigInt,              /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanRealToBigInt,              /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualRealToBigInt,         /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanRealToBigInt,           /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualRealToBigInt       /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            dtlPhysicalFuncIsEqualRealToReal,                   /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualRealToReal,                /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanRealToReal,                /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualRealToReal,           /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanRealToReal,             /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualRealToReal         /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlPhysicalFuncIsEqualRealToDouble,                 /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualRealToDouble,              /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanRealToDouble,              /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualRealToDouble,         /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanRealToDouble,           /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualRealToDouble       /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlPhysicalFuncIsEqualRealToNumeric,               /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualRealToNumeric,            /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanRealToNumeric,            /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualRealToNumeric,       /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanRealToNumeric,         /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualRealToNumeric     /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            dtlPhysicalFuncIsEqualRealToNumeric,               /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualRealToNumeric,            /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanRealToNumeric,            /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualRealToNumeric,       /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanRealToNumeric,         /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualRealToNumeric     /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualDoubleToSmallInt,             /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualDoubleToSmallInt,          /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanDoubleToSmallInt,          /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualDoubleToSmallInt,     /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanDoubleToSmallInt,       /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualDoubleToSmallInt   /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlPhysicalFuncIsEqualDoubleToInteger,              /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualDoubleToInteger,           /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanDoubleToInteger,           /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualDoubleToInteger,      /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanDoubleToInteger,        /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualDoubleToInteger    /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlPhysicalFuncIsEqualDoubleToBigInt,               /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualDoubleToBigInt,            /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanDoubleToBigInt,            /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualDoubleToBigInt,       /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanDoubleToBigInt,         /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualDoubleToBigInt            /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {   
            dtlPhysicalFuncIsEqualDoubleToReal,                 /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualDoubleToReal,              /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanDoubleToReal,              /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualDoubleToReal,         /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanDoubleToReal,           /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualDoubleToReal       /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlPhysicalFuncIsEqualDoubleToDouble,               /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualDoubleToDouble,            /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanDoubleToDouble,            /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualDoubleToDouble,       /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanDoubleToDouble,         /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualDoubleToDouble     /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlPhysicalFuncIsEqualDoubleToNumeric,                 /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualDoubleToNumeric,              /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanDoubleToNumeric,              /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualDoubleToNumeric,         /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanDoubleToNumeric,           /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualDoubleToNumeric       /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            dtlPhysicalFuncIsEqualDoubleToNumeric,                 /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualDoubleToNumeric,              /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanDoubleToNumeric,              /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualDoubleToNumeric,         /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanDoubleToNumeric,           /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualDoubleToNumeric       /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualNumericToSmallInt,                 /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToSmallInt,              /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToSmallInt,              /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToSmallInt,         /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToSmallInt,           /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToSmallInt       /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlPhysicalFuncIsEqualNumericToInteger,                   /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToInteger,                /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToInteger,                /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToInteger,           /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToInteger,             /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToInteger         /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlPhysicalFuncIsEqualNumericToBigInt,                    /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToBigInt,                 /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToBigInt,                 /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToBigInt,            /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToBigInt,              /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToBigInt          /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {     
            dtlPhysicalFuncIsEqualNumericToReal,                      /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToReal,                   /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToReal,                   /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToReal,              /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToReal,                /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToReal            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlPhysicalFuncIsEqualNumericToDouble,                    /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToDouble,                 /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToDouble,                 /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToDouble,            /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToDouble,              /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToDouble          /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlPhysicalFuncIsEqualNumericToNumeric,                   /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToNumeric,                /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToNumeric,                /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToNumeric,           /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToNumeric,             /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToNumeric         /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            dtlPhysicalFuncIsEqualNumericToNumeric,                   /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToNumeric,                /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToNumeric,                /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToNumeric,           /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToNumeric,             /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToNumeric         /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualNumericToSmallInt,                 /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToSmallInt,              /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToSmallInt,              /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToSmallInt,         /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToSmallInt,           /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToSmallInt       /**< IS GREATER THAN EQUAL */
        },

        /**< INTEGER */
        {   
            dtlPhysicalFuncIsEqualNumericToInteger,                   /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToInteger,                /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToInteger,                /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToInteger,           /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToInteger,             /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToInteger         /**< IS GREATER THAN EQUAL */
        },

        /**< BIGINT */
        {   
            dtlPhysicalFuncIsEqualNumericToBigInt,                    /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToBigInt,                 /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToBigInt,                 /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToBigInt,            /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToBigInt,              /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToBigInt          /**< IS GREATER THAN EQUAL */
        },

        /**< REAL */
        {     
            dtlPhysicalFuncIsEqualNumericToReal,                      /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToReal,                   /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToReal,                   /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToReal,              /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToReal,                /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToReal            /**< IS GREATER THAN EQUAL */
        },

        /**< DOUBLE PRECISION */
        {   
            dtlPhysicalFuncIsEqualNumericToDouble,                    /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToDouble,                 /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToDouble,                 /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToDouble,            /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToDouble,              /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToDouble          /**< IS GREATER THAN EQUAL */
        },

        /**< FLOAT */
        {   
            dtlPhysicalFuncIsEqualNumericToNumeric,                   /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToNumeric,                /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToNumeric,                /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToNumeric,           /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToNumeric,             /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToNumeric         /**< IS GREATER THAN EQUAL */
        },
        
        /**< NUMBER */
        {   
            dtlPhysicalFuncIsEqualNumericToNumeric,                   /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualNumericToNumeric,                /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanNumericToNumeric,                /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualNumericToNumeric,           /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanNumericToNumeric,             /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualNumericToNumeric         /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualFixedLengthChar,              /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualFixedLengthChar,           /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanFixedLengthChar,           /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualFixedLengthChar,      /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanFixedLengthChar,        /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualFixedLengthChar    /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            dtlPhysicalFuncIsEqualVariableLengthChar,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualVariableLengthChar,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanVariableLengthChar,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualVariableLengthChar,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanVariableLengthChar,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualVariableLengthChar /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            dtlPhysicalFuncIsEqualCharAndLongvarchar,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualCharAndLongvarchar,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanCharAndLongvarchar,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualCharAndLongvarchar,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanCharAndLongvarchar,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualCharAndLongvarchar /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualVariableLengthChar,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualVariableLengthChar,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanVariableLengthChar,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualVariableLengthChar,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanVariableLengthChar,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualVariableLengthChar /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            dtlPhysicalFuncIsEqualVariableLengthChar,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualVariableLengthChar,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanVariableLengthChar,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualVariableLengthChar,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanVariableLengthChar,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualVariableLengthChar /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            dtlPhysicalFuncIsEqualVarcharAndLongvarchar,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualVarcharAndLongvarchar,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanVarcharAndLongvarchar,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualVarcharAndLongvarchar,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanVarcharAndLongvarchar,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualVarcharAndLongvarchar /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualLongvarcharAndChar,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualLongvarcharAndChar,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanLongvarcharAndChar,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualLongvarcharAndChar,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanLongvarcharAndChar,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualLongvarcharAndChar /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER VARYING */
        {   
            dtlPhysicalFuncIsEqualLongvarcharAndVarchar,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualLongvarcharAndVarchar,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanLongvarcharAndVarchar,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualLongvarcharAndVarchar,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanLongvarcharAndVarchar,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualLongvarcharAndVarchar /**< IS GREATER THAN EQUAL */
        },

        /**< CHARACTER LONG VARYING */
        {   
            dtlPhysicalFuncIsEqualLongvarcharAndLongvarchar,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualLongvarcharAndLongvarchar,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanLongvarcharAndLongvarchar,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualLongvarcharAndLongvarchar,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanLongvarcharAndLongvarchar,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualLongvarcharAndLongvarchar /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualFixedLengthBinary,            /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualFixedLengthBinary,         /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanFixedLengthBinary,         /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualFixedLengthBinary,    /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanFixedLengthBinary,      /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualFixedLengthBinary  /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            dtlPhysicalFuncIsEqualVariableLengthBinary,         /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualVariableLengthBinary,      /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanVariableLengthBinary,      /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualVariableLengthBinary, /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanVariableLengthBinary,   /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualVariableLengthBinary /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            dtlPhysicalFuncIsEqualBinaryAndLongvarbinary,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualBinaryAndLongvarbinary,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanBinaryAndLongvarbinary,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualBinaryAndLongvarbinary,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanBinaryAndLongvarbinary,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualBinaryAndLongvarbinary /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualVariableLengthBinary,         /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualVariableLengthBinary,      /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanVariableLengthBinary,      /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualVariableLengthBinary, /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanVariableLengthBinary,   /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualVariableLengthBinary /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            dtlPhysicalFuncIsEqualVariableLengthBinary,         /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualVariableLengthBinary,      /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanVariableLengthBinary,      /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualVariableLengthBinary, /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanVariableLengthBinary,   /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualVariableLengthBinary /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            dtlPhysicalFuncIsEqualVarbinaryAndLongvarbinary,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualVarbinaryAndLongvarbinary,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanVarbinaryAndLongvarbinary,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualVarbinaryAndLongvarbinary,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanVarbinaryAndLongvarbinary,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualVarbinaryAndLongvarbinary /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualLongvarbinaryAndBinary,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualLongvarbinaryAndBinary,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanLongvarbinaryAndBinary,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualLongvarbinaryAndBinary,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanLongvarbinaryAndBinary,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualLongvarbinaryAndBinary /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY VARYING */
        {   
            dtlPhysicalFuncIsEqualLongvarbinaryAndVarbinary,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualLongvarbinaryAndVarbinary,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanLongvarbinaryAndVarbinary,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualLongvarbinaryAndVarbinary,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanLongvarbinaryAndVarbinary,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualLongvarbinaryAndVarbinary /**< IS GREATER THAN EQUAL */
        },

        /**< BINARY LONG VARYING */
        {   
            dtlPhysicalFuncIsEqualLongvarbinaryAndLongvarbinary,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualLongvarbinaryAndLongvarbinary,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanLongvarbinaryAndLongvarbinary,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualLongvarbinaryAndLongvarbinary,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanLongvarbinaryAndLongvarbinary,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualLongvarbinaryAndLongvarbinary /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualDateToDate,                   /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualDateToDate,                /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanDateToDate,                /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualDateToDate,           /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanDateToDate,             /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualDateToDate         /**< IS GREATER THAN EQUAL */
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
            dtlPhysicalFuncIsEqualDateToTimestamp,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualDateToTimestamp,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanDateToTimestamp,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualDateToTimestamp,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanDateToTimestamp,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualDateToTimestamp /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualTimeToTime,                   /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualTimeToTime,                /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanTimeToTime,                /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualTimeToTime,           /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanTimeToTime,             /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualTimeToTime         /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualTimestampToDate,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualTimestampToDate,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanTimestampToDate,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualTimestampToDate,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanTimestampToDate,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualTimestampToDate /**< IS GREATER THAN EQUAL */
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
            dtlPhysicalFuncIsEqualTimestampToTimestamp,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualTimestampToTimestamp,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanTimestampToTimestamp,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualTimestampToTimestamp,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanTimestampToTimestamp,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualTimestampToTimestamp /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualTimeTzToTimeTz,               /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualTimeTzToTimeTz,            /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanTimeTzToTimeTz,            /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualTimeTzToTimeTz,       /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanTimeTzToTimeTz,         /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualTimeTzToTimeTz     /**< IS GREATER THAN EQUAL */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
            dtlPhysicalFuncIsEqualTimestampTzToTimestampTz,     /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualTimestampTzToTimestampTz,  /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanTimestampTzToTimestampTz,  /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualTimestampTzToTimestampTz,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanTimestampTzToTimestampTz,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualTimestampTzToTimestampTz /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
 * INTERVAL YEAR TO MONTH
 ******************************************************************************/
    /**< INTERVAL YEAR TO MONTH         */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            dtlPhysicalFuncIsEqualIntervalYearToMonth,          /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualIntervalYearToMonth,       /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanIntervalYearToMonth,       /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualIntervalYearToMonth,  /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanIntervalYearToMonth,    /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualIntervalYearToMonth /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
 * INTERVAL DAY TO SECOND
 ******************************************************************************/
    /**< INTERVAL DAY TO SECOND         */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            dtlPhysicalFuncIsEqualIntervalDayToSecond,          /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualIntervalDayToSecond,       /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanIntervalDayToSecond,       /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualIntervalDayToSecond,  /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanIntervalDayToSecond,    /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualIntervalDayToSecond /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
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
 * ROWID
 ******************************************************************************/
    /**< INTERVAL DAY TO SECOND         */
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

        /**< INTERVAL YEAR TO MONTH */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< INTERVAL DAY TO SECOND */
        {   
            NULL,           /**< IS EQUAL */
            NULL,           /**< IS NOT EQUAL */
            NULL,           /**< IS LESS THAN */
            NULL,           /**< IS LESS THAN EQUAL */
            NULL,           /**< IS GREATER THAN */
            NULL            /**< IS GREATER THAN EQUAL */
        },

        /**< ROWID */
        {   
            dtlPhysicalFuncIsEqualRowIdToRowId,           /**< IS EQUAL */
            dtlPhysicalFuncIsNotEqualRowIdToRowId,        /**< IS NOT EQUAL */
            dtlPhysicalFuncIsLessThanRowIdToRowId,        /**< IS LESS THAN */
            dtlPhysicalFuncIsLessThanEqualRowIdToRowId,   /**< IS LESS THAN EQUAL */
            dtlPhysicalFuncIsGreaterThanRowIdToRowId,     /**< IS GREATER THAN */
            dtlPhysicalFuncIsGreaterThanEqualRowIdToRowId /**< IS GREATER THAN EQUAL */
        }        
    }   
};


 /** @} dtlPhysicalFunc */

