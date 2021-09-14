/*******************************************************************************
 * ellDictBuiltInDataType.c
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
 * @file ellDictBuiltInDataType.c
 * @brief Built-In Data Type 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldBuiltInDataType.h>

/**
 * @addtogroup ellBuiltInDataType 
 * @{
 */

/**
 * @brief Built-In Data Type Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aTypeName                Type Name
 * @param[in]  aODBCDataType            ODBC Data Type
 * @param[in]  aJDBCDataType            JDBC Data Type
 * @param[in]  aColumnSize              Column Size
 * @param[in]  aLiteralPrefix           Literal Prefix
 * @param[in]  aLiteralSuffix           Literal Suffix
 * @param[in]  aCreateParams            Create Params
 * @param[in]  aNullable                Nullable
 * @param[in]  aCaseSensitive           Case Sensitive
 * @param[in]  aSearchable              Searchable
 * @param[in]  aUnsignedAttribute       Unsigned Attribute
 * @param[in]  aFixedPrecScale          Fixed Precisoin Scale
 * @param[in]  aAutoUniqueValue         Auto Unique Value
 * @param[in]  aLocalTypeName           Local Type Name
 * @param[in]  aMinimumScale            Numeric Minimum Scale or Fractional Second Min Precision
 * @param[in]  aMaximumScale            Numeric Maximum Scale or Fractional Second Max Precision
 * @param[in]  aSQLDataType             SQL Data Type
 * @param[in]  aSQLDateTimeSub          SQL DateTime Sub Code
 * @param[in]  aNumPrecRadix            Numeric의 기본 Radix
 * @param[in]  aIntervalPrecision       Interval Max Precision
 * @param[in]  aSupportedFeature        지원 여부
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 * DB 생성시에만 사용된다.
 */
stlStatus ellInsertBuiltInTypeDesc( smlTransId                      aTransID,
                                    smlStatement                  * aStmt,
                                    stlChar                       * aTypeName,
                                    stlInt32                        aODBCDataType,
                                    stlInt32                        aJDBCDataType,
                                    stlInt32                        aColumnSize,
                                    stlChar                       * aLiteralPrefix,
                                    stlChar                       * aLiteralSuffix,
                                    stlChar                       * aCreateParams,
                                    stlInt32                        aNullable,
                                    stlBool                         aCaseSensitive,
                                    dtlSearchable                   aSearchable,
                                    stlInt32                        aUnsignedAttribute,
                                    stlBool                         aFixedPrecScale,
                                    stlInt32                        aAutoUniqueValue,
                                    stlChar                       * aLocalTypeName,
                                    stlInt32                        aMinimumScale,
                                    stlInt32                        aMaximumScale,
                                    stlInt32                        aSQLDataType,
                                    stlInt32                        aSQLDateTimeSub,
                                    dtlNumericPrecRadix             aNumPrecRadix,
                                    stlInt32                        aIntervalPrecision,
                                    stlBool                         aSupportedFeature,
                                    ellEnv                        * aEnv )
{
    STL_TRY( eldInsertBuiltInTypeDesc( aTransID,
                                       aStmt,
                                       aTypeName,
                                       aODBCDataType,
                                       aJDBCDataType,
                                       aColumnSize,
                                       aLiteralPrefix,
                                       aLiteralSuffix,
                                       aCreateParams,
                                       aNullable,
                                       aCaseSensitive,
                                       aSearchable,
                                       aUnsignedAttribute,
                                       aFixedPrecScale,
                                       aAutoUniqueValue,
                                       aLocalTypeName,
                                       aMinimumScale,
                                       aMaximumScale,
                                       aSQLDataType,
                                       aSQLDateTimeSub,
                                       aNumPrecRadix,
                                       aIntervalPrecision,
                                       aSupportedFeature,
                                       aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/** @} ellBuiltInDataType */
