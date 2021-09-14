/*******************************************************************************
 * zlciParameter.c
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

#include <cml.h>
#include <goldilocks.h>

#include <zlDef.h>
#include <zli.h>

#include <zlcr.h>

/**
 * @file zlciParameter.c
 * @brief ODBC API Internal Parameter Routines.
 */

/**
 * @addtogroup zlci
 * @{
 */

stlStatus zlciInitParameterBlock( zlsStmt       * aStmt,
                                  stlInt32      * aInitParamCount,
                                  stlErrorStack * aErrorStack )
{
    zldDesc              * sApd;
    zldDesc              * sIpd;
    zldDescElement       * sApdRec;
    zldDescElement       * sIpdRec;
    dtlDataType            sDtlDataType;
    stlInt64               sArg1;
    stlInt64               sArg2;
    dtlStringLengthUnit    sStringLengthUnit;
    dtlIntervalIndicator   sIntervalIndicator;
    stlInt32               sIdx = 0;
    stlInt32               sInitParamCount = 0;

    sIpd = &aStmt->mIpd;
    sApd = aStmt->mApd;
    
    STL_TRY_THROW( sIpd->mCount > 0, RAMP_SUCCESS );

    sApdRec = STL_RING_GET_FIRST_DATA( &sApd->mDescRecRing );

    STL_RING_FOREACH_ENTRY( &sIpd->mDescRecRing, sIpdRec )
    {
        sIdx++;
        
        if( sIpdRec->mParameterType == SQL_PARAM_TYPE_UNKNOWN )
        {
            sApdRec = STL_RING_GET_NEXT_DATA( &sApd->mDescRecRing, sApdRec);
            continue;
        }

        /*
         * Parameter 정보 변경이 없으면 skip
         */
        if( ( sIpdRec->mChanged != STL_TRUE ) &&
            ( sApd->mChanged != STL_TRUE ) )
        {
            sApdRec = STL_RING_GET_NEXT_DATA( &sApd->mDescRecRing, sApdRec);
            continue;
        }

        sInitParamCount++;

        STL_TRY( zliInitParamBlock( aStmt,
                                    sIpd,
                                    sApdRec,
                                    sIpdRec,
                                    &sDtlDataType,
                                    &sArg1,
                                    &sArg2,
                                    &sStringLengthUnit,
                                    &sIntervalIndicator,
                                    aErrorStack )
                 == STL_SUCCESS );

        sApdRec = STL_RING_GET_NEXT_DATA( &sApd->mDescRecRing, sApdRec);
    }

    STL_RAMP( RAMP_SUCCESS );

    *aInitParamCount = sInitParamCount;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlciExecuteParameterType( zlsStmt       * aStmt,
                                    stlInt8       * aControl,
                                    stlErrorStack * aErrorStack )
{
    cmlHandle            * sHandle;
    zlcDbc               * sDbc;
    zldDesc              * sApd;
    zldDesc              * sIpd;
    zldDescElement       * sApdRec;
    zldDescElement       * sIpdRec;
    dtlDataType            sDtlDataType;
    stlInt64               sArg1;
    stlInt64               sArg2;
    dtlStringLengthUnit    sStringLengthUnit;
    dtlIntervalIndicator   sIntervalIndicator;
    stlInt32               sIdx = 0;
    stlInt16               sOutParamCount = 0;

    sDbc    = aStmt->mParentDbc;
    sHandle = &sDbc->mComm;
    
    sIpd = &aStmt->mIpd;
    sApd = aStmt->mApd;
    
    STL_TRY_THROW( sIpd->mCount > 0, RAMP_SUCCESS );

    sApdRec = STL_RING_GET_FIRST_DATA( &sApd->mDescRecRing );

    STL_RING_FOREACH_ENTRY( &sIpd->mDescRecRing, sIpdRec )
    {
        sIdx++;

        switch( sIpdRec->mParameterType )
        {
            case SQL_PARAM_INPUT :
                break;
            case SQL_PARAM_INPUT_OUTPUT :                
            case SQL_PARAM_OUTPUT :
                sOutParamCount++;
                break;
            case SQL_PARAM_TYPE_UNKNOWN :
                sApdRec = STL_RING_GET_NEXT_DATA( &sApd->mDescRecRing, sApdRec);
                continue;
            default :
                STL_ASSERT( STL_FALSE );
                break;
        }

        /*
         * Parameter 정보 변경이 없으면 skip
         */
        if( ( sIpdRec->mChanged != STL_TRUE ) &&
            ( sApd->mChanged != STL_TRUE ) )
        {
            sApdRec = STL_RING_GET_NEXT_DATA( &sApd->mDescRecRing, sApdRec);
            continue;
        }

        STL_TRY( zliGetDtlDataType( sIpdRec->mConsiceType,
                                    &sDtlDataType,
                                    aErrorStack ) == STL_SUCCESS );

        zliGetDtlDataArgs( sIpdRec, &sArg1, &sArg2 );

        sStringLengthUnit  = zliGetDtlDataUnit( aStmt,
                                                sIpdRec,
                                                sApdRec->mConsiceType,
                                                sIpdRec->mConsiceType );
        sIntervalIndicator = zliGetDtlDataIndicator( sIpdRec->mConsiceType );

        STL_TRY( cmlWriteBindTypeCommand( sHandle,
                                          *aControl,
                                          aStmt->mStmtId,
                                          sIdx,
                                          sIpdRec->mParameterType,
                                          (stlInt16)sDtlDataType,
                                          sArg1,
                                          sArg2,
                                          (stlInt8)sStringLengthUnit,
                                          (stlInt8)sIntervalIndicator,
                                          aErrorStack )
                 == STL_SUCCESS );

        *aControl = CML_CONTROL_NONE;

        sApdRec = STL_RING_GET_NEXT_DATA( &sApd->mDescRecRing, sApdRec);
    }

    aStmt->mOutParamCount = sOutParamCount;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlciSetParameterComplete( zlsStmt       * aStmt,
                                    stlErrorStack * aErrorStack )
{
    zldDesc        * sApd;
    zldDesc        * sIpd;
    zldDescElement * sIpdRec;

    sIpd = &aStmt->mIpd;
    sApd = aStmt->mApd;
    
    STL_TRY_THROW( sIpd->mCount > 0, RAMP_SUCCESS );

    STL_RING_FOREACH_ENTRY( &sIpd->mDescRecRing, sIpdRec )
    {
        sIpdRec->mChanged = STL_FALSE;
    }

    sApd->mChanged = STL_FALSE;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
}

stlStatus zlciExecuteParameterBlock( zlsStmt       * aStmt,
                                     stlInt8       * aControl,
                                     stlInt32      * aExecParamCount,
                                     stlErrorStack * aErrorStack )
{
    cmlHandle      * sHandle;
    zlcDbc         * sDbc;
    zldDesc        * sIpd;
    zldDescElement * sIpdRec;
    stlInt32         sExecParamCount = 0;
    stlInt32         sAllColCount = 0;

    sDbc    = aStmt->mParentDbc;
    sHandle = &sDbc->mComm;
    
    sIpd = &aStmt->mIpd;
    
    STL_TRY_THROW( sIpd->mCount > 0, RAMP_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( &sIpd->mDescRecRing, sIpdRec )
    {
        sAllColCount++;
        switch( sIpdRec->mParameterType )
        {
            case SQL_PARAM_INPUT :
            case SQL_PARAM_INPUT_OUTPUT :
                sExecParamCount++;
                break;
            default :
                break;
        }
    }

    /* in, inout 컬럼이 있을 경우에만 bind data를 기록한다. */
    if( sExecParamCount > 0 )
    {
        STL_TRY( cmlWriteBindDataBeginCommand( sHandle,
                                               *aControl,
                                               aStmt->mStmtId,
                                               sAllColCount, /* out bind column 개수도 포함해야 한다. */
                                               sIpd->mArraySize,
                                               aErrorStack )
                 == STL_SUCCESS );

        *aControl = CML_CONTROL_NONE;

        sIpd = &aStmt->mIpd;
    
        STL_RING_FOREACH_ENTRY( &sIpd->mDescRecRing, sIpdRec )
        {
            switch( sIpdRec->mParameterType )
            {
                case SQL_PARAM_INPUT :
                case SQL_PARAM_INPUT_OUTPUT :                
                    STL_TRY( cmlWriteBindDataCommand( sHandle,
                                                      STL_FALSE, /* out bind or unknown */
                                                      sIpd->mArraySize,
                                                      DTL_GET_BLOCK_FIRST_DATA_VALUE( sIpdRec->mInputValueBlock ),
                                                      aErrorStack )
                             == STL_SUCCESS );
                    break;
                default :
                    STL_TRY( cmlWriteBindDataCommand( sHandle,
                                                      STL_TRUE, /* out bind or unknown */
                                                      sIpd->mArraySize,
                                                      DTL_GET_BLOCK_FIRST_DATA_VALUE( sIpdRec->mInputValueBlock ),
                                                      aErrorStack )
                             == STL_SUCCESS );
                    break;
            }
        }
    }

    STL_RAMP( RAMP_SUCCESS );

    *aExecParamCount = sExecParamCount;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlciExecuteOutParameterBlock( zlsStmt       * aStmt,
                                        stlBool         aLast,
                                        stlErrorStack * aErrorStack )
{
    cmlHandle      * sHandle;
    zlcDbc         * sDbc;
    zldDesc        * sIpd;
    zldDescElement * sIpdRec;
    stlInt32         sIdx = 0;
    stlInt8          sControl = CML_CONTROL_NONE;
    stlInt16         sOutParamCount = 0;

    sDbc   = aStmt->mParentDbc;
    sHandle = &sDbc->mComm;
    
    sIpd = &aStmt->mIpd;
    
    STL_TRY_THROW( sIpd->mCount > 0, RAMP_SUCCESS );

    STL_RING_FOREACH_ENTRY( &sIpd->mDescRecRing, sIpdRec )
    {
        sIdx++;
        
        switch( sIpdRec->mParameterType )
        {
            case SQL_PARAM_OUTPUT :
            case SQL_PARAM_INPUT_OUTPUT :
                sOutParamCount++;

                if( (aLast == STL_TRUE) && (sOutParamCount == aStmt->mOutParamCount) )
                {
                    sControl = CML_CONTROL_TAIL;
                }

                STL_TRY( cmlWriteGetOutBindDataCommand( sHandle,
                                                        sControl,
                                                        aStmt->mStmtId,
                                                        sIdx,
                                                        aErrorStack )
                         == STL_SUCCESS );
                break;
            default :
                break;
        }
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlciGetOutParameterBlock( zlsStmt       * aStmt,
                                    stlBool       * aIgnored,
                                    stlErrorStack * aErrorStack )
{
    cmlHandle      * sHandle;
    zlcDbc         * sDbc;
    zldDesc        * sIpd;
    zldDescElement * sIpdRec;
    zldDescElement * sIpdRecNext;

    sDbc    = aStmt->mParentDbc;
    sHandle = &sDbc->mComm;
    
    sIpd = &aStmt->mIpd;
    
    STL_TRY_THROW( sIpd->mCount > 0, RAMP_SUCCESS );

    STL_RING_FOREACH_ENTRY_SAFE( &sIpd->mDescRecRing, sIpdRec, sIpdRecNext )
    {
        switch( sIpdRec->mParameterType )
        {
            case SQL_PARAM_OUTPUT :
            case SQL_PARAM_INPUT_OUTPUT :                
                STL_TRY( cmlReadGetOutBindDataResult( sHandle,
                                                      aIgnored,
                                                      DTL_GET_BLOCK_FIRST_DATA_VALUE( sIpdRec->mOutputValueBlock ),
                                                      zlcrReallocLongVaryingMem,
                                                      (void*)aStmt,
                                                      aErrorStack ) == STL_SUCCESS );
                break;
            default :
                break;
        }
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    STL_RING_AT_FOREACH_ENTRY( &sIpd->mDescRecRing, sIpdRecNext, sIpdRec )
    {
        switch( sIpdRec->mParameterType )
        {
            case SQL_PARAM_OUTPUT :
            case SQL_PARAM_INPUT_OUTPUT :                
                (void)cmlReadGetOutBindDataResult( sHandle,
                                                   aIgnored,
                                                   DTL_GET_BLOCK_FIRST_DATA_VALUE( sIpdRec->mOutputValueBlock ),
                                                   zlcrReallocLongVaryingMem,
                                                   (void*)aStmt,
                                                   aErrorStack );
                break;
            default :
                break;
        }
    }
    
    return STL_FAILURE;
}

stlStatus zlciNumParams( zlcDbc        * aDbc,
                         zlsStmt       * aStmt,
                         stlInt16      * aParameterCount,
                         stlErrorStack * aErrorStack )
{
    cmlHandle * sHandle  = &aDbc->mComm;
    stlBool     sIgnored = STL_FALSE;
    stlInt32    sParameterCount;

    STL_TRY( cmlWriteNumParamsCommand( sHandle,
                                       CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                       aStmt->mStmtId,
                                       aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlSendPacket( sHandle,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlReadNumParamsResult( sHandle,
                                     &sIgnored,
                                     &sParameterCount,
                                     aErrorStack ) == STL_SUCCESS );

    if( aParameterCount != NULL )
    {
        *aParameterCount = (stlInt16)sParameterCount;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlciGetParameterType( zlcDbc        * aDbc,
                                zlsStmt       * aStmt,
                                stlUInt16       aParamIdx,
                                stlInt16      * aInputOutputType,
                                stlErrorStack * aErrorStack )
                                    
{
    cmlHandle * sHandle = &aDbc->mComm;
    stlBool     sIgnored = STL_FALSE;
    stlInt16    sInputOutputType = SQL_PARAM_TYPE_UNKNOWN;

    STL_TRY( cmlWriteParameterTypeCommand( sHandle,
                                           CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                           aStmt->mStmtId,
                                           aParamIdx,
                                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlSendPacket( sHandle,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlReadParameterTypeResult( sHandle,
                                         &sIgnored,
                                         &sInputOutputType,
                                         aErrorStack ) == STL_SUCCESS );

    if( aInputOutputType != NULL )
    {
        *aInputOutputType = sInputOutputType;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
