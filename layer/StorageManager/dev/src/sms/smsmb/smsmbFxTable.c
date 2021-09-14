/*******************************************************************************
 * smsmbFxTable.c
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

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smt.h>
#include <smxm.h>
#include <smxl.h>
#include <smp.h>
#include <smg.h>
#include <smsDef.h>
#include <smsmbDef.h>
#include <smsmbFxTable.h>
#include <smsmbSegment.h>

extern smsWarmupEntry * gSmsWarmupEntry;

/**
 * @file smsmbFxTable.c
 * @brief Storage Manager Layer Memory Bitmap Segment Fixed Table Internal Routines
 */

/**
 * @addtogroup smsmbFxTable
 * @{
 */

knlFxColumnDesc gSmsmbBitmapColumnDesc[] =
{
    {
        "IS_LEAF",
        "is leaf",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smsmbFxBitmap, mIsLeaf ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "DEPTH",
        "depth in tree",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smsmbFxBitmap, mDepth ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PAGE_ID",
        "page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smsmbFxBitmap, mPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "SLOT_SEQ",
        "slot sequence",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smsmbFxBitmap, mOffset ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "CHILD_PID",
        "offset",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smsmbFxBitmap, mChildPid ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "FREENESS",
        "freeness",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smsmbFxBitmap, mFreeness ),
        2,
        STL_FALSE  /* nullable */
    },
    {
        "AGABLE_SCN",
        "agable scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smsmbFxBitmap, mAgableScn ),
        STL_SIZEOF(smlScn),
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

stlStatus smsmbOpenBitmapCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv )
{
    smsmbFxBitmapIterator * sIterator;
    smsmbCache            * sCache;
    smsmbFxBitmap           sFxBitmap;

    sIterator = (smsmbFxBitmapIterator*)aPathCtrl;
    sCache = (smsmbCache*)aDumpObjHandle;
    sIterator->mTbsId = sCache->mCommonCache.mTbsId;
    sIterator->mSearchStackTop = -1;

    sFxBitmap.mChildPid = sCache->mRootNode;
    sFxBitmap.mIsLeaf = STL_FALSE;
    sFxBitmap.mDepth = 0;
    
    STL_TRY( smsmbPushChildPage( sIterator,
                                 &sFxBitmap,
                                 (smlEnv*)aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbCloseBitmapCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smsmbGetDumpObjectCallback( stlInt64   aTransId,
                                      void     * aStmt,
                                      stlChar  * aDumpObjName,
                                      void    ** aDumpObjHandle,
                                      knlEnv   * aEnv )
{
    stlBool    sExist = STL_TRUE;
    stlChar  * sToken1;
    stlChar  * sToken2;
    stlChar  * sToken3;
    stlChar  * sObjName = NULL;
    stlInt32   sTokenCount;
    void     * sRelationHandle;
    smlRelationType   sRelType;

    *aDumpObjHandle = NULL;

    STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );
    
    if( stlStrcmp( sToken1, "TABLE" ) == 0 )
    {
        STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                            &sToken1,
                                            &sToken2,
                                            &sToken3,
                                            &sTokenCount,
                                            aEnv ) == STL_SUCCESS );
        if( sTokenCount == 1 )
        {
            sObjName = sToken1;
            STL_TRY( gKnlDumpNameHelper.mGetBaseTableHandle( aTransId,
                                                             ((smlStatement*) aStmt)->mScn,
                                                             NULL,
                                                             sToken1,
                                                             NULL,
                                                             &sRelationHandle,
                                                             &sExist,
                                                             aEnv ) == STL_SUCCESS );
        }
        else if( sTokenCount == 2 )
        {
            sObjName = sToken2;
            STL_TRY( gKnlDumpNameHelper.mGetBaseTableHandle( aTransId,
                                                             ((smlStatement*) aStmt)->mScn,
                                                             sToken1,
                                                             sToken2,
                                                             NULL,
                                                             &sRelationHandle,
                                                             &sExist,
                                                             aEnv ) == STL_SUCCESS );
        }
        else
        {
            sExist = STL_FALSE;
        }

        if( sExist == STL_TRUE )
        {
            sRelType = SME_GET_RELATION_TYPE( sRelationHandle );
            STL_TRY_THROW( sRelType == SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE,
                           RAMP_ERR_INVALID_ARGUMENT );
        }
    }
    else if( stlStrcmp( sToken1, "INDEX" ) == 0 )
    {
        STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                            &sToken1,
                                            &sToken2,
                                            &sToken3,
                                            &sTokenCount,
                                            aEnv ) == STL_SUCCESS );
        if( sTokenCount == 1 )
        {
            STL_TRY( gKnlDumpNameHelper.mGetIndexHandle( aTransId,
                                                         ((smlStatement*) aStmt)->mScn,
                                                         NULL,
                                                         sToken1,
                                                         NULL,
                                                         &sRelationHandle,
                                                         &sExist,
                                                         aEnv ) == STL_SUCCESS );
        }
        else if( sTokenCount == 2 )
        {
            STL_TRY( gKnlDumpNameHelper.mGetIndexHandle( aTransId,
                                                         ((smlStatement*) aStmt)->mScn,
                                                         sToken1,
                                                         sToken2,
                                                         NULL,
                                                         &sRelationHandle,
                                                         &sExist,
                                                         aEnv ) == STL_SUCCESS );
        }
        else
        {
            sExist = STL_FALSE;
        }
    }
    else
    {
        sExist = STL_FALSE;
    }

    STL_TRY_THROW( sExist == STL_TRUE, RAMP_FINISH );

    *aDumpObjHandle = SME_GET_SEGMENT_HANDLE( sRelationHandle );
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ARGUMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_DUMP_OPTION_STRING,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sObjName );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbBuildRecordBitmapCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aEnv )
{
    smsmbFxBitmapIterator * sIterator;
    smsmbFxBitmap         * sFxBitmap;

    sIterator = (smsmbFxBitmapIterator*)aPathCtrl;
    sFxBitmap = smsmbPopFxBitmap( sIterator );

    *aTupleExist = STL_FALSE;
    
    STL_TRY_THROW( sFxBitmap != NULL, RAMP_SUCCESS );
    
    STL_TRY( knlBuildFxRecord( gSmsmbBitmapColumnDesc,
                               sFxBitmap,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );
    
    *aTupleExist = STL_TRUE;

    if( sFxBitmap->mIsLeaf != STL_TRUE )
    {
        STL_TRY( smsmbPushChildPage( sIterator,
                                     sFxBitmap,
                                     (smlEnv*)aEnv )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbPushChildPage( smsmbFxBitmapIterator * aIterator,
                              smsmbFxBitmap         * aFxBitmap,
                              smlEnv                * aEnv )
{
    smsmbFxBitmap      sFxBitmap;
    smpHandle          sPageHandle;
    stlInt32           i;
    smpPageType        sPageType;
    smpFreeness        sFreeness;
    smsmbInternalHdr * sInternalHdr;
    smsmbInternal    * sInternal;
    smsmbLeafHdr     * sLeafHdr;
    smsmbLeaf        * sLeaf;
    stlInt32           sState = 0;
    stlChar            sDepth;

    STL_TRY( smpFix( aIterator->mTbsId,
                     aFxBitmap->mChildPid,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    sDepth = aFxBitmap->mDepth + 1;

    sPageType = smpGetPageType( &sPageHandle );

    if( sPageType == SMP_PAGE_TYPE_BITMAP_INTERNAL )
    {
        sInternalHdr = (smsmbInternalHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sInternal = (smsmbInternal*)((stlChar*)sInternalHdr + STL_SIZEOF(smsmbInternalHdr));

        for( i = (SMSMB_INTERNAL_BIT_COUNT-1); i >= 0; i-- )
        {
            sFreeness = smsmbGetFreeness( sInternal->mFreeness, i );

            switch( sFreeness )
            {
                case SMP_FREENESS_UNALLOCATED:
                    stlStrcpy(sFxBitmap.mFreeness, "UA" );
                    break;
                case SMP_FREENESS_FULL:
                    stlStrcpy(sFxBitmap.mFreeness, "FU" );
                    break;
                case SMP_FREENESS_UPDATEONLY:
                    stlStrcpy(sFxBitmap.mFreeness, "UP" );
                    break;
                case SMP_FREENESS_INSERTABLE:
                    stlStrcpy(sFxBitmap.mFreeness, "IN" );
                    break;
                case SMP_FREENESS_FREE:
                    stlStrcpy(sFxBitmap.mFreeness, "FR" );
                    break;
                default:
                    break;
            }
            sFxBitmap.mPageId = smpGetPageId( &sPageHandle );
            sFxBitmap.mChildPid = sInternal->mChildPid[i];
            sFxBitmap.mOffset = i;
            sFxBitmap.mIsLeaf = STL_FALSE;
            sFxBitmap.mDepth = sDepth;
            sFxBitmap.mAgableScn = 0;

            if( sFreeness != SMP_FREENESS_UNALLOCATED )
            {
                smsmbPushFxBitmap( aIterator, sFxBitmap );
            }
        }
    }
    else
    {
        sLeafHdr = (smsmbLeafHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sLeaf = (smsmbLeaf*)((stlChar*)sLeafHdr + STL_SIZEOF(smsmbLeafHdr));
    
        for( i = (SMSMB_LEAF_BIT_COUNT-1); i >= 0; i-- )
        {
            sFreeness = smsmbGetFreeness( sLeaf->mFreeness, i );

            switch( sFreeness )
            {
                case SMP_FREENESS_UNALLOCATED:
                    stlStrcpy(sFxBitmap.mFreeness, "UA" );
                    break;
                case SMP_FREENESS_FULL:
                    stlStrcpy(sFxBitmap.mFreeness, "FU" );
                    break;
                case SMP_FREENESS_UPDATEONLY:
                    stlStrcpy(sFxBitmap.mFreeness, "UP" );
                    break;
                case SMP_FREENESS_INSERTABLE:
                    stlStrcpy(sFxBitmap.mFreeness, "IN" );
                    break;
                case SMP_FREENESS_FREE:
                    stlStrcpy(sFxBitmap.mFreeness, "FR" );
                    break;
                default:
                    break;
            }
            sFxBitmap.mPageId = smpGetPageId( &sPageHandle );
            sFxBitmap.mChildPid = sLeaf->mDataPid[i];
            sFxBitmap.mOffset = i;
            sFxBitmap.mIsLeaf = STL_TRUE;
            sFxBitmap.mDepth = sDepth;
            sFxBitmap.mAgableScn = sLeaf->mScn[i];
                
            if( sFreeness != SMP_FREENESS_UNALLOCATED )
            {
               smsmbPushFxBitmap( aIterator, sFxBitmap );
            }
        }
    }

    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

void smsmbPushFxBitmap( smsmbFxBitmapIterator * aIterator,
                        smsmbFxBitmap           aFxBitmap )
{
    aIterator->mSearchStackTop++;
    aIterator->mSearchStack[aIterator->mSearchStackTop] = aFxBitmap;
}

smsmbFxBitmap * smsmbPopFxBitmap( smsmbFxBitmapIterator * aIterator )
{
    smsmbFxBitmap * sFxBitmap = NULL;

    if( aIterator->mSearchStackTop >= 0 )
    {
        sFxBitmap = &(aIterator->mSearchStack[aIterator->mSearchStackTop]);
        aIterator->mSearchStackTop--;
    }

    return sFxBitmap;
}

knlFxTableDesc gSmsmbBitmapTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smsmbGetDumpObjectCallback,
    smsmbOpenBitmapCallback,
    smsmbCloseBitmapCallback,
    smsmbBuildRecordBitmapCallback,
    STL_SIZEOF( smsmbFxBitmapIterator ),
    "D$MEMORY_SEGMENT_BITMAP",
    "memory segment bitmap information",
    gSmsmbBitmapColumnDesc
};

knlFxColumnDesc gSmsmbBitmapHdrColumnDesc[] =
{
    {
        "IS_LEAF",
        "is leaf",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smsmbFxBitmapHdr, mIsLeaf ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "DEPTH",
        "depth in tree",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smsmbFxBitmapHdr, mDepth ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PAGE_ID",
        "page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smsmbFxBitmapHdr, mPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "NEXT_PAGE_ID",
        "page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smsmbFxBitmapHdr, mNextPid ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "CHILD_COUNT",
        "count of child",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smsmbFxBitmapHdr, mChildCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "FREENESS",
        "freeness",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smsmbFxBitmapHdr, mFreeness ),
        2,
        STL_FALSE  /* nullable */
    },
    {
        "UNALLOCATED_BIT_COUNT",
        "Unallocated bit count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smsmbFxBitmapHdr, mUnallocatedBitCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "FULL_BIT_COUNT",
        "Full bit count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smsmbFxBitmapHdr, mFullBitCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "UPDATEONLY_BIT_COUNT",
        "Update-Only bit count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smsmbFxBitmapHdr, mUpdateonlyBitCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "INSERTABLE_BIT_COUNT",
        "Insertable bit count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smsmbFxBitmapHdr, mInsertableBitCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "FREE_BIT_COUNT",
        "Free bit count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smsmbFxBitmapHdr, mFreeBitCount ),
        STL_SIZEOF( stlInt16 ),
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

stlStatus smsmbOpenBitmapHdrCallback( void   * aStmt,
                                      void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv )
{
    smsmbFxBitmapHdrIterator * sIterator;
    smsmbCache               * sCache;
    smsmbFxBitmapHdr           sFxBitmapHdr;

    sIterator = (smsmbFxBitmapHdrIterator*)aPathCtrl;
    sCache = (smsmbCache*)aDumpObjHandle;
    sIterator->mTbsId = sCache->mCommonCache.mTbsId;
    sIterator->mSearchStackTop = -1;

    SMSMB_INIT_FX_BITMAP_HDR( &sFxBitmapHdr );
    
    sFxBitmapHdr.mPageId = sCache->mRootNode;
    sFxBitmapHdr.mDepth = 0;
    
    smsmbPushFxBitmapHdr( sIterator, sFxBitmapHdr );
    
    return STL_SUCCESS;
}

stlStatus smsmbBuildRecordBitmapHdrCallback( void              * aDumpObjHandle,
                                             void              * aPathCtrl,
                                             knlValueBlockList * aValueList,
                                             stlInt32            aBlockIdx,
                                             stlBool           * aTupleExist,
                                             knlEnv            * aEnv )
{
    smsmbFxBitmapHdrIterator * sIterator;
    smsmbFxBitmapHdr         * sFxBitmapHdr;

    sIterator = (smsmbFxBitmapHdrIterator*)aPathCtrl;
    sFxBitmapHdr = smsmbPopFxBitmapHdr( sIterator );

    *aTupleExist = STL_FALSE;
    
    STL_TRY_THROW( sFxBitmapHdr != NULL, RAMP_SUCCESS );

    STL_TRY( smsmbBuildBitmapHdr( sIterator,
                                  sFxBitmapHdr,
                                  (smlEnv*)aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlBuildFxRecord( gSmsmbBitmapHdrColumnDesc,
                               sFxBitmapHdr,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );
    
    *aTupleExist = STL_TRUE;

    if( sFxBitmapHdr->mIsLeaf != STL_TRUE )
    {
        STL_TRY( smsmbPushChildPageHdr( sIterator,
                                        sFxBitmapHdr,
                                        (smlEnv*)aEnv )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbBuildBitmapHdr( smsmbFxBitmapHdrIterator * aIterator,
                               smsmbFxBitmapHdr         * aFxBitmapHdr,
                               smlEnv                   * aEnv )
{
    smpHandle          sPageHandle;
    smpPageType        sPageType;
    smsmbInternalHdr * sInternalHdr;
    smsmbLeafHdr     * sLeafHdr;
    stlInt32           sState = 0;
    smpPhyHdr        * sPhyHdr;

    STL_TRY( smpFix( aIterator->mTbsId,
                     aFxBitmapHdr->mPageId,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    sPageType = smpGetPageType( &sPageHandle );
    sPhyHdr = (smpPhyHdr*)SMP_FRAME( &sPageHandle );
    
    switch( sPhyHdr->mFreeness )
    {
        case SMP_FREENESS_UNALLOCATED:
            stlStrcpy(aFxBitmapHdr->mFreeness, "UA" );
            break;
        case SMP_FREENESS_FULL:
            stlStrcpy(aFxBitmapHdr->mFreeness, "FU" );
            break;
        case SMP_FREENESS_UPDATEONLY:
            stlStrcpy(aFxBitmapHdr->mFreeness, "UP" );
            break;
        case SMP_FREENESS_INSERTABLE:
            stlStrcpy(aFxBitmapHdr->mFreeness, "IN" );
            break;
        case SMP_FREENESS_FREE:
            stlStrcpy(aFxBitmapHdr->mFreeness, "FR" );
            break;
        default:
            break;
    }
        
    if( sPageType == SMP_PAGE_TYPE_BITMAP_INTERNAL )
    {
        sInternalHdr = (smsmbInternalHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

        aFxBitmapHdr->mChildCount = sInternalHdr->mChildCount;
        aFxBitmapHdr->mIsLeaf = STL_FALSE;
        aFxBitmapHdr->mUnallocatedBitCount = sInternalHdr->mFreenessCount[SMP_FREENESS_UNALLOCATED];
        aFxBitmapHdr->mFullBitCount = sInternalHdr->mFreenessCount[SMP_FREENESS_FULL];
        aFxBitmapHdr->mUpdateonlyBitCount = sInternalHdr->mFreenessCount[SMP_FREENESS_UPDATEONLY];
        aFxBitmapHdr->mInsertableBitCount = sInternalHdr->mFreenessCount[SMP_FREENESS_INSERTABLE];
        aFxBitmapHdr->mFreeBitCount = sInternalHdr->mFreenessCount[SMP_FREENESS_FREE];
        aFxBitmapHdr->mNextPid = SMP_NULL_PID;
    }
    else
    {
        sLeafHdr = (smsmbLeafHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
    
        aFxBitmapHdr->mChildCount = sLeafHdr->mChildCount;
        aFxBitmapHdr->mIsLeaf = STL_TRUE;
        aFxBitmapHdr->mUnallocatedBitCount = sLeafHdr->mFreenessCount[SMP_FREENESS_UNALLOCATED];
        aFxBitmapHdr->mFullBitCount = sLeafHdr->mFreenessCount[SMP_FREENESS_FULL];
        aFxBitmapHdr->mUpdateonlyBitCount = sLeafHdr->mFreenessCount[SMP_FREENESS_UPDATEONLY];
        aFxBitmapHdr->mInsertableBitCount = sLeafHdr->mFreenessCount[SMP_FREENESS_INSERTABLE];
        aFxBitmapHdr->mFreeBitCount = sLeafHdr->mFreenessCount[SMP_FREENESS_FREE];
        aFxBitmapHdr->mNextPid = sLeafHdr->mNextPid;
    }

    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmbPushChildPageHdr( smsmbFxBitmapHdrIterator * aIterator,
                                 smsmbFxBitmapHdr         * aFxBitmapHdr,
                                 smlEnv                   * aEnv )
{
    smsmbFxBitmapHdr   sFxBitmapHdr;
    smpHandle          sPageHandle;
    stlInt32           i;
    smsmbInternalHdr * sInternalHdr;
    smsmbInternal    * sInternal;
    stlInt32           sState = 0;
    stlChar            sDepth;

    STL_TRY( smpFix( aIterator->mTbsId,
                     aFxBitmapHdr->mPageId,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    sDepth = aFxBitmapHdr->mDepth + 1;

    sInternalHdr = (smsmbInternalHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
    sInternal = (smsmbInternal*)((stlChar*)sInternalHdr + STL_SIZEOF(smsmbInternalHdr));

    for( i = (sInternalHdr->mChildCount-1); i >= 0; i-- )
    {
        SMSMB_INIT_FX_BITMAP_HDR( &sFxBitmapHdr );
    
        sFxBitmapHdr.mPageId = sInternal->mChildPid[i];
        sFxBitmapHdr.mDepth = sDepth;
                
        smsmbPushFxBitmapHdr( aIterator, sFxBitmapHdr );
    }
    
    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

void smsmbPushFxBitmapHdr( smsmbFxBitmapHdrIterator * aIterator,
                           smsmbFxBitmapHdr           aFxBitmapHdr )
{
    aIterator->mSearchStackTop++;
    aIterator->mSearchStack[aIterator->mSearchStackTop] = aFxBitmapHdr;
}

smsmbFxBitmapHdr * smsmbPopFxBitmapHdr( smsmbFxBitmapHdrIterator * aIterator )
{
    smsmbFxBitmapHdr * sFxBitmapHdr = NULL;

    if( aIterator->mSearchStackTop >= 0 )
    {
        sFxBitmapHdr = &(aIterator->mSearchStack[aIterator->mSearchStackTop]);
        aIterator->mSearchStackTop--;
    }

    return sFxBitmapHdr;
}

knlFxTableDesc gSmsmbBitmapHdrTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smsmbGetDumpObjectCallback,
    smsmbOpenBitmapHdrCallback,
    smsmbCloseBitmapCallback,
    smsmbBuildRecordBitmapHdrCallback,
    STL_SIZEOF( smsmbFxBitmapHdrIterator ),
    "D$MEMORY_SEGMENT_BITMAP_HEADER",
    "memory segment bitmap header information",
    gSmsmbBitmapHdrColumnDesc
};

/** @} */
