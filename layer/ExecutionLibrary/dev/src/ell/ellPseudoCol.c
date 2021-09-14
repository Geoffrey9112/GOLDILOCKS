/*******************************************************************************
 * ellPseudoCol.c
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
 * @file ellPseudoCol.c
 * @brief Execution Library Layer Pseudo Column Routines
 */

#include <ell.h>


/**
 * @ingroup ellPseudoCol
 * @{
 */


ellPseudoColInfo gPseudoColInfoArr[ KNL_PSEUDO_COL_MAX ] =
{
    /**
     * {
     *     ID,
     *     IterationTime,
     *     Name,
     *     Data Type,
     *     ArgNum1,
     *     ArgNum2,
     *     StringLengthUnit,
     *     IntervalIndicator,
     *     Min Argument Count,
     *     Max Argument Count,
     *     Function Pointer
     * }
     */

    {
        KNL_PSEUDO_COL_INVALID,
        DTL_ITERATION_TIME_INVALID,
        "INVALID",
        DTL_TYPE_NA,
        DTL_PRECISION_NA,
        DTL_SCALE_NA,
        DTL_STRING_LENGTH_UNIT_NA,
        DTL_INTERVAL_INDICATOR_NA,
        0,
        0,
        NULL
    },

    /*************************************************************
     * Sequence
     *************************************************************/
    
    {
        KNL_PSEUDO_COL_CURRVAL,
        DTL_ITERATION_TIME_FOR_EACH_EXPR,
        "CURRVAL",
        DTL_TYPE_NATIVE_BIGINT,
        DTL_PRECISION_NA,
        DTL_SCALE_NA,
        DTL_STRING_LENGTH_UNIT_NA,
        DTL_INTERVAL_INDICATOR_NA,
        1,
        1,
        ellPseudoSequenceCurrVal
    },

    {
        KNL_PSEUDO_COL_NEXTVAL,
        DTL_ITERATION_TIME_FOR_EACH_EXPR,
        "NEXTVAL",
        DTL_TYPE_NATIVE_BIGINT,
        DTL_PRECISION_NA,
        DTL_SCALE_NA,
        DTL_STRING_LENGTH_UNIT_NA,
        DTL_INTERVAL_INDICATOR_NA,
        1,
        1,
        ellPseudoSequenceNextVal
    },

    
    /*************************************************************
     * ROWID
     *************************************************************/

    {
        KNL_PSEUDO_COL_ROWID,
        DTL_ITERATION_TIME_FOR_EACH_EXPR,  
        "ROWID",
        DTL_TYPE_ROWID,
        DTL_PRECISION_NA,
        DTL_SCALE_NA,
        DTL_STRING_LENGTH_UNIT_NA,
        DTL_INTERVAL_INDICATOR_NA,
        0,
        0,
        ellPseudoRowId
    },

    
    /*************************************************************
     * ROWNUM
     *************************************************************/

    {
        KNL_PSEUDO_COL_ROWNUM,
        DTL_ITERATION_TIME_FOR_EACH_EXPR,
        "ROWNUM",
        DTL_TYPE_ROWID,
        DTL_PRECISION_NA,
        DTL_SCALE_NA,
        DTL_STRING_LENGTH_UNIT_NA,
        DTL_INTERVAL_INDICATOR_NA,
        0,
        0,
        ellPseudoRowId
    }
};


/**
 * @brief CURRVAL(seq) 수행 
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aSmlRid             SmlRid
 * @param[in]  aTableID            Table ID
 * @param[in]  aInputArgumentCnt   Input Argument Count 
 * @param[in]  aInputArgument      Input Arguments
 * @param[out] aResultValue        Result Value
 * @param[in]  aEnv                Environment
 */
stlStatus ellPseudoSequenceCurrVal( smlTransId       aTransID,
                                    smlStatement   * aStmt,
                                    smlRid         * aSmlRid,
                                    stlInt64         aTableID,
                                    stlUInt16        aInputArgumentCnt,
                                    ellPseudoArg   * aInputArgument,
                                    dtlDataValue   * aResultValue,
                                    ellEnv         * aEnv )
{
    ellPseudoColInfo  * sColInfo          = NULL;
    stlBool             sSucceessWithInfo = STL_FALSE;

    void    * sSeqPhyHandle = NULL;
    stlInt64  sSeqValue = 0;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInputArgumentCnt == 1, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInputArgument != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultValue != NULL, ELL_ERROR_STACK(aEnv) );

    
    /**
     * Pseudo Column 정보 획득
     */
    
    sColInfo  = & gPseudoColInfoArr[ KNL_PSEUDO_COL_CURRVAL ];
    sSeqPhyHandle = (void *) aInputArgument[0].mPseudoArg;

    STL_TRY( smlGetCurrSessSequenceVal( sSeqPhyHandle,
                                        & sSeqValue,
                                        SML_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Result 생성 
     */
    
    STL_TRY( dtlSetValueFromInteger( sColInfo->mDataType,
                                     sSeqValue,
                                     sColInfo->mArgNum1,
                                     sColInfo->mArgNum2,
                                     sColInfo->mStringLengthUnit,
                                     sColInfo->mIntervalIndicator,
                                     gResultDataTypeDef[sColInfo->mDataType].mDataValueSize,
                                     aResultValue,
                                     & sSucceessWithInfo,
                                     KNL_DT_VECTOR(aEnv),
                                     aEnv,
                                     ELL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
             
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NEXTVAL(seq) 수행 
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aSmlRid             SmlRid
 * @param[in]  aTableID            Table ID
 * @param[in]  aInputArgumentCnt   Input Argument Count 
 * @param[in]  aInputArgument      Input Arguments
 * @param[out] aResultValue        Result Value
 * @param[in]  aEnv                Environment
 */
stlStatus ellPseudoSequenceNextVal( smlTransId       aTransID,
                                    smlStatement   * aStmt,
                                    smlRid         * aSmlRid,
                                    stlInt64         aTableID,
                                    stlUInt16        aInputArgumentCnt,
                                    ellPseudoArg   * aInputArgument,
                                    dtlDataValue   * aResultValue,
                                    ellEnv         * aEnv )
{
    ellPseudoColInfo  * sColInfo          = NULL;
    stlBool             sSucceessWithInfo = STL_FALSE;

    void    * sSeqPhyHandle = NULL;
    stlInt64  sSeqValue = 0;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInputArgumentCnt == 1, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInputArgument != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultValue != NULL, ELL_ERROR_STACK(aEnv) );

    
    /**
     * Pseudo Column 정보 획득
     */
    
    sColInfo  = & gPseudoColInfoArr[ KNL_PSEUDO_COL_NEXTVAL ];
    sSeqPhyHandle = (void *) aInputArgument[0].mPseudoArg;

    STL_TRY( smlGetNextSequenceVal( sSeqPhyHandle,
                                    & sSeqValue,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Result 생성 
     */
    
    STL_TRY( dtlSetValueFromInteger( sColInfo->mDataType,
                                     sSeqValue,
                                     sColInfo->mArgNum1,
                                     sColInfo->mArgNum2,
                                     sColInfo->mStringLengthUnit,
                                     sColInfo->mIntervalIndicator,
                                     gResultDataTypeDef[sColInfo->mDataType].mDataValueSize,
                                     aResultValue,
                                     & sSucceessWithInfo,
                                     KNL_DT_VECTOR(aEnv),
                                     aEnv,
                                     ELL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
             
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ROWID 수행 
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aSmlRid             SmlRid
 * @param[in]  aTableID            Table ID
 * @param[in]  aInputArgumentCnt   Input Argument Count 
 * @param[in]  aInputArgument      Input Arguments
 * @param[out] aResultValue        Result Value
 * @param[in]  aEnv                Environment
 */
stlStatus ellPseudoRowId( smlTransId       aTransID,
                          smlStatement   * aStmt,
                          smlRid         * aSmlRid,
                          stlInt64         aTableID,
                          stlUInt16        aInputArgumentCnt,
                          ellPseudoArg   * aInputArgument,
                          dtlDataValue   * aResultValue,
                          ellEnv         * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSmlRid != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInputArgumentCnt == 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultValue != NULL, ELL_ERROR_STACK(aEnv) );

    
    /**
     * Result 생성
     */

    STL_TRY( dtlRowIdSetValueFromRowIdInfo( aTableID,
                                            aSmlRid->mTbsId,
                                            aSmlRid->mPageId,
                                            aSmlRid->mOffset,
                                            aResultValue,
                                            ELL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
                                            
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} ellPseudoCol */
 
