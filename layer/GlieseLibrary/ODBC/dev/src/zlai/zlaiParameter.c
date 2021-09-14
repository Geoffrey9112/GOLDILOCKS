/*******************************************************************************
 * zlaiParameter.c
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

#include <goldilocks.h>
#include <cml.h>
#include <ssl.h>
#include <zlDef.h>
#include <zli.h>
#include <zlai.h>

/**
 * @file zlaiParameter.c
 * @brief ODBC API Internal Parameter Routines.
 */

/**
 * @addtogroup zlai
 * @{
 */

stlStatus zlaiInitParameterBlock( zlsStmt   * aStmt,
                                  stlInt32  * aInitParamCount,
                                  void      * aEnv )
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
    stlInt32               sIdx = 1;
    stlInt32               sInitParamCount = 0;
    stlInt64               sBufferSize = 0;
    sslEnv               * sEnv = (sslEnv*)aEnv;

    sIpd = &aStmt->mIpd;
    sApd = aStmt->mApd;
    
    STL_TRY_THROW( sIpd->mCount > 0, RAMP_SUCCESS );

    sApdRec = STL_RING_GET_FIRST_DATA( &sApd->mDescRecRing );

    STL_RING_FOREACH_ENTRY( &sIpd->mDescRecRing, sIpdRec )
    {
        if( sIpdRec->mParameterType == SQL_PARAM_TYPE_UNKNOWN )
        {
            sIdx++;
            sApdRec = STL_RING_GET_NEXT_DATA( &sApd->mDescRecRing, sApdRec);
            continue;
        }

        /*
         * Parameter 정보 변경이 없으면 skip
         */
        if( ( sIpdRec->mChanged != STL_TRUE ) &&
            ( sApd->mChanged != STL_TRUE ) )
        {
            sIdx++;
            sApdRec = STL_RING_GET_NEXT_DATA( &sApd->mDescRecRing, sApdRec);
            continue;
        }

        sInitParamCount++;

        sIpdRec->mInputValueBlock  = NULL;
        sIpdRec->mOutputValueBlock = NULL;
        
        switch( sIpdRec->mParameterType )
        {
            case SQL_PARAM_INPUT :
            case SQL_PARAM_INPUT_OUTPUT :
            case SQL_PARAM_INPUT_OUTPUT_STREAM :
            STL_TRY( zliInitParamBlock( aStmt,
                                        sIpd,
                                        sApdRec,
                                        sIpdRec,
                                        &sDtlDataType,
                                        &sArg1,
                                        &sArg2,
                                        &sStringLengthUnit,
                                        &sIntervalIndicator,
                                        KNL_ERROR_STACK(sEnv) )
                     == STL_SUCCESS );
            
            sIpdRec->mOutputValueBlock = NULL;

            STL_TRY( sslCreateParameter( aStmt->mStmtId,
                                         sIdx,
                                         sIpdRec->mParameterType,
                                         sDtlDataType,
                                         sArg1,
                                         sArg2,
                                         sStringLengthUnit,
                                         sIntervalIndicator,
                                         &sIpdRec->mInputValueBlock,
                                         &sIpdRec->mOutputValueBlock,
                                         sEnv ) == STL_SUCCESS );
            break;
            case SQL_PARAM_OUTPUT :
            case SQL_PARAM_OUTPUT_STREAM :
                zliGetDtlDataArgs( sIpdRec, &sArg1, &sArg2 );
            
                STL_TRY( zliGetDtlDataType( sIpdRec->mConsiceType,
                                            &sDtlDataType,
                                            KNL_ERROR_STACK(sEnv) ) == STL_SUCCESS );

                sStringLengthUnit  = zliGetDtlDataUnit( aStmt,
                                                        sIpdRec,
                                                        sApdRec->mConsiceType,
                                                        sIpdRec->mConsiceType );
                sIntervalIndicator = zliGetDtlDataIndicator( sIpdRec->mConsiceType );

                STL_TRY( dtlGetDataValueBufferSize( sDtlDataType,
                                                    sArg1,
                                                    sStringLengthUnit,
                                                    &sBufferSize,
                                                    ZLS_STMT_DT_VECTOR( aStmt ),
                                                    (void*)aStmt,
                                                    KNL_ERROR_STACK(sEnv) )
                         == STL_SUCCESS );

                sIpdRec->mOctetLength = sBufferSize;

                STL_TRY( sslCreateParameter( aStmt->mStmtId,
                                             sIdx,
                                             sIpdRec->mParameterType,
                                             sDtlDataType,
                                             sArg1,
                                             sArg2,
                                             sStringLengthUnit,
                                             sIntervalIndicator,
                                             &sIpdRec->mInputValueBlock,
                                             &sIpdRec->mOutputValueBlock,
                                             sEnv ) == STL_SUCCESS );
                break;
            default :
                STL_ASSERT( STL_FALSE );
                break;
        }

        sIpdRec->mChanged = STL_FALSE;

        sIdx++;
        sApdRec = STL_RING_GET_NEXT_DATA( &sApd->mDescRecRing, sApdRec);
    }

    sApd->mChanged = STL_FALSE;

    STL_RAMP( RAMP_SUCCESS );

    *aInitParamCount = sInitParamCount;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlaiNumParams( zlcDbc        * aDbc,
                         zlsStmt       * aStmt,
                         stlInt16      * aParameterCount,
                         stlErrorStack * aErrorStack )
{
    stlInt32   sState = 0;
    sslEnv   * sEnv;
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );

    STL_TRY( zlaiNumParamsInternal( aDbc,
                                    aStmt,
                                    aParameterCount,
                                    sEnv )
             == STL_SUCCESS );

    sState = 0;
    sslLeaveSession( aDbc->mSessionId,
                     aDbc->mSessionSeq );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
            STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
            sslLeaveSession( aDbc->mSessionId, aDbc->mSessionSeq );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus zlaiNumParamsInternal( zlcDbc    * aDbc,
                                 zlsStmt   * aStmt,
                                 stlInt16  * aParameterCount,
                                 void      * aEnv )
{
    stlInt16   sParameterCount;
    sslEnv   * sEnv = (sslEnv*)aEnv;

    STL_TRY( sslNumParams( aStmt->mStmtId,
                           &sParameterCount,
                           sEnv )
             == STL_SUCCESS );

    if( aParameterCount != NULL )
    {
        *aParameterCount = sParameterCount;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlaiGetParameterType( zlcDbc        * aDbc,
                                zlsStmt       * aStmt,
                                stlUInt16       aParamIdx,
                                stlInt16      * aInputOutputType,
                                stlErrorStack * aErrorStack )
                                    
{
    stlInt32   sState = 0;
    sslEnv   * sEnv;
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    STL_TRY( sslGetParameterType( aStmt->mStmtId,
                                  aParamIdx,
                                  aInputOutputType,
                                  sEnv ) == STL_SUCCESS );

    sState = 0;
    sslLeaveSession( aDbc->mSessionId,
                     aDbc->mSessionSeq );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
            STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
            sslLeaveSession( aDbc->mSessionId, aDbc->mSessionSeq );
        default:
            break;
    }

    return STL_FAILURE;
}

/** @} */
