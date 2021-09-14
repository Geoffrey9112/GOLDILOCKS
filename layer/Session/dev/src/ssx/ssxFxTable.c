/*******************************************************************************
 * ssxFxTable.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ssxFxTable.c 9474 2013-08-29 03:22:39Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <qll.h>
#include <sslDef.h>
#include <ssDef.h>
#include <ssx.h>

extern ssgWarmupEntry * gSsgWarmupEntry;

/**
 * @file ssxFxTable.c
 * @brief Session Layer Global Transaction Fixed Table Internal Routines
 */

/**
 * @addtogroup ssxFxTable
 * @{
 */

knlFxColumnDesc gSsxGlobalTransColumnDesc[] =
{
    {
        "GLOBAL_TRANS_ID",
        "global transaction identifier",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( ssxFxGlobalTrans, mXid ),
        STL_XID_DATA_SIZE * 3,
        STL_FALSE  /* nullable */
    },
    {
        "LOCAL_TRANS_ID",
        "local transaction identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( ssxFxGlobalTrans, mTransId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "STATE",
        "global transaction state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( ssxFxGlobalTrans, mState ),
        32,
        STL_FALSE  /* nullable */
    },
    {
        "ASSO_STATE",
        "associate state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( ssxFxGlobalTrans, mAssociateState ),
        32,
        STL_FALSE  /* nullable */
    },
    {
        "START_TIME",
        "transaction start time",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( ssxFxGlobalTrans, mBeginTime ),
        STL_SIZEOF( stlTime ),
        STL_FALSE  /* nullable */
    },
    {
        "REPREPARABLE",
        "re-preparable transaction",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( ssxFxGlobalTrans, mRepreparable ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};

stlStatus ssxOpenGlobalTransCallback( void   * aStmt,
                                      void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv )
{
    ssxFxGlobalTransPathCtrl * sPathCtrl;

    sPathCtrl = (ssxFxGlobalTransPathCtrl*)aPathCtrl;
    
    sPathCtrl->mCurContextArray = STL_RING_GET_FIRST_DATA( &gSsgWarmupEntry->mContextArrList );
    sPathCtrl->mCurPosInContextArray = 0;
    
    return STL_SUCCESS;
}

stlStatus ssxCloseGlobalTransCallback( void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus ssxBuildRecordGlobalTransCallback( void              * aDumpObjHandle,
                                             void              * aPathCtrl,
                                             knlValueBlockList * aValueList,
                                             stlInt32            aBlockIdx,
                                             stlBool           * aTupleExist,
                                             knlEnv            * aEnv )
{
    ssxFxGlobalTransPathCtrl * sPathCtrl;
    knlXaContext               sContext;
    ssxFxGlobalTrans           sFxGlobalTrans;

    sPathCtrl = (ssxFxGlobalTransPathCtrl*)aPathCtrl;
    *aTupleExist = STL_TRUE;
    
    while( 1 )
    {
        if( sPathCtrl->mCurPosInContextArray >= SSX_CONTEXT_ARRAY_SIZE )
        {
            sPathCtrl->mCurContextArray =
                STL_RING_GET_NEXT_DATA( &gSsgWarmupEntry->mContextArrList,
                                        sPathCtrl->mCurContextArray );
            sPathCtrl->mCurPosInContextArray = 0;

            if( sPathCtrl->mCurContextArray == (ssxContextArray*)&gSsgWarmupEntry->mContextArrList )
            {
                *aTupleExist = STL_FALSE;
                break;
            }
        }
        
        stlMemcpy( &sContext,
                   &sPathCtrl->mCurContextArray->mContext[sPathCtrl->mCurPosInContextArray],
                   STL_SIZEOF(knlXaContext) );

        if( sContext.mState == KNL_XA_STATE_NOTR )
        {
            sPathCtrl->mCurPosInContextArray++;
            continue;
        }
        
        sFxGlobalTrans.mTransId = sContext.mTransId;
        sFxGlobalTrans.mBeginTime = smlBeginTransTime( sContext.mTransId, SML_ENV(aEnv) );
        sFxGlobalTrans.mRepreparable = smlIsRepreparableTransaction( sContext.mTransId );

        STL_TRY( dtlXidToString( &sContext.mXid,
                                 STL_XID_DATA_SIZE * 3,
                                 sFxGlobalTrans.mXid,
                                 KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        
        switch( sContext.mState )
        {
            case KNL_XA_STATE_NOTR:
                stlStrcpy( sFxGlobalTrans.mState, "NOTR" );
                break;
            case KNL_XA_STATE_ACTIVE:
                stlStrcpy( sFxGlobalTrans.mState, "ACTIVE" );
                break;
            case KNL_XA_STATE_IDLE:
                stlStrcpy( sFxGlobalTrans.mState, "IDLE" );
                break;
            case KNL_XA_STATE_PREPARED:
                stlStrcpy( sFxGlobalTrans.mState, "PREPARED" );
                break;
            case KNL_XA_STATE_ROLLBACK_ONLY:
                stlStrcpy( sFxGlobalTrans.mState, "ROLLBACK_ONLY" );
                break;
            case KNL_XA_STATE_HEURISTIC_COMPLETED:
                stlStrcpy( sFxGlobalTrans.mState, "HEURISTIC_COMPLETED" );
                break;
            default:
                stlStrcpy( sFxGlobalTrans.mState, "UNKNOWN" );
                break;
        }

        switch( sContext.mAssociateState )
        {
            case KNL_XA_ASSOCIATE_STATE_FALSE:
                stlStrcpy( sFxGlobalTrans.mAssociateState, "NOT_ASSOCIATED" );
                break;
            case KNL_XA_ASSOCIATE_STATE_TRUE:
                stlStrcpy( sFxGlobalTrans.mAssociateState, "ASSOCIATED" );
                break;
            case KNL_XA_ASSOCIATE_STATE_SUSPEND:
                stlStrcpy( sFxGlobalTrans.mAssociateState, "ASSOCIATION_SUSPENDED" );
                break;
            default:
                stlStrcpy( sFxGlobalTrans.mAssociateState, "UNKNOWN" );
                break;
        }

        STL_TRY( knlBuildFxRecord( gSsxGlobalTransColumnDesc,
                                   &sFxGlobalTrans,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );
        
        sPathCtrl->mCurPosInContextArray++;
        break;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSsxGlobalTransTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    ssxOpenGlobalTransCallback,
    ssxCloseGlobalTransCallback,
    ssxBuildRecordGlobalTransCallback,
    STL_SIZEOF( ssxFxGlobalTransPathCtrl ),
    "X$GLOBAL_TRANSACTION",
    "global transaction information",
    gSsxGlobalTransColumnDesc
};


/** @} */
