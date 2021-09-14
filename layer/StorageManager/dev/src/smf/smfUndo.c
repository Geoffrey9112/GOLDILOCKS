/*******************************************************************************
 * smfUndo.c
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
#include <smfDef.h>
#include <smf.h>
#include <smxm.h>
#include <smfUndo.h>
#include <smfRecovery.h>

extern smfDatafileFuncs * gDatafileFuncs[];

/**
 * @file smfUndo.c
 * @brief Storage Manager Datafile Undo Internal Routines
 */

/**
 * @addtogroup smfUndo
 * @{
 */

smxlUndoFunc gSmfUndoFuncs[SMF_UNDO_LOG_MAX_COUNT] =
{
    smfUndoMemoryFileCreate,
    smfUndoMemoryFileDrop,
    smfUndoMemoryTbsCreate,
    smfUndoMemoryTbsDrop,
    smfUndoMemoryDatafileAdd,
    smfUndoMemoryDatafileDrop,
    smfUndoMemoryTbsRename
};

stlStatus smfUndoMemoryFileCreate( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv )
{
    stlInt32            sTbsTypeId;
    stlInt32            sTbsPhysicalId;
    stlInt32            sHintTbsId;
    stlInt32            sLogOffset = 0;
    smfDatafilePersData sDatafilePersData;

    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( SML_TBS_DEVICE_MEMORY );
    
    STL_READ_MOVE_INT32( &sTbsPhysicalId, aLogBody, sLogOffset );
    STL_READ_MOVE_INT32( &sHintTbsId, aLogBody, sLogOffset );
    STL_READ_MOVE_BYTES( &sDatafilePersData,
                         aLogBody,
                         STL_SIZEOF(smfDatafilePersData),
                         sLogOffset );
    
    STL_TRY( gDatafileFuncs[sTbsTypeId]->mCreateUndo( smxmGetTransId( aMiniTrans ),
                                                      sTbsPhysicalId,
                                                      sHintTbsId,
                                                      &sDatafilePersData,
                                                      STL_FALSE,  /* For Restore */
                                                      STL_TRUE,   /* aClrLogging */
                                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfUndoMemoryFileDrop( smxmTrans * aMiniTrans,
                                 smlRid      aTargetRid,
                                 void      * aLogBody,
                                 stlInt16    aLogSize,
                                 smlEnv    * aEnv )
{
    stlInt32            sTbsTypeId;
    stlInt32            sTbsPhysicalId;
    stlInt32            sLogOffset = 0;
    smfDatafilePersData sDatafilePersData;

    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( SML_TBS_DEVICE_MEMORY );
    
    STL_READ_MOVE_INT32( &sTbsPhysicalId, aLogBody, sLogOffset );
    STL_READ_MOVE_BYTES( &sDatafilePersData,
                         aLogBody,
                         STL_SIZEOF(smfDatafilePersData),
                         sLogOffset );

    STL_TRY( gDatafileFuncs[sTbsTypeId]->mDropUndo( smxmGetTransId( aMiniTrans ),
                                                    sTbsPhysicalId,
                                                    SMF_INVALID_PHYSICAL_TBS_ID, /* aHintTbsId */
                                                    &sDatafilePersData,
                                                    STL_FALSE,  /* For Restore */
                                                    STL_FALSE,  /* aClrLogging */
                                                    aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfUndoMemoryTbsCreate( smxmTrans * aMiniTrans,
                                  smlRid      aTargetRid,
                                  void      * aLogBody,
                                  stlInt16    aLogSize,
                                  smlEnv    * aEnv )
{
    stlInt32       sTbsTypeId;
    smfTbsPersData sTbsPersData;

    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( SML_TBS_DEVICE_MEMORY );
    
    STL_WRITE_BYTES( &sTbsPersData,
                     aLogBody,
                     STL_SIZEOF(smfTbsPersData) );
    
    STL_TRY( smfCreateTbsUndo( smxmGetTransId( aMiniTrans ),
                               sTbsTypeId,
                               &sTbsPersData,
                               STL_TRUE, /* aClrLogging */
                               aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfUndoMemoryTbsDrop( smxmTrans * aMiniTrans,
                                smlRid      aTargetRid,
                                void      * aLogBody,
                                stlInt16    aLogSize,
                                smlEnv    * aEnv )
{
    smfTbsPersData sTbsPersData;
    
    STL_WRITE_BYTES( &sTbsPersData,
                     aLogBody,
                     STL_SIZEOF(smfTbsPersData) );
    
    STL_TRY( smfDropTbsUndo( smxmGetTransId( aMiniTrans ),
                             &sTbsPersData,
                             STL_TRUE, /* aClrLogging */
                             aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfUndoMemoryDatafileAdd( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv )
{
    smfDatafilePersData sDatafilePersData;
    
    STL_WRITE_BYTES( &sDatafilePersData,
                     aLogBody,
                     STL_SIZEOF(smfDatafilePersData) );
    
    STL_TRY( smfAddDatafileUndo( smxmGetTransId( aMiniTrans ),
                                 SML_TBS_DEVICE_TYPE_ID( SML_TBS_DEVICE_MEMORY ),
                                 aTargetRid.mTbsId,
                                 &sDatafilePersData,
                                 STL_TRUE, /* aClrLogging */
                                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfUndoMemoryDatafileDrop( smxmTrans * aMiniTrans,
                                     smlRid      aTargetRid,
                                     void      * aLogBody,
                                     stlInt16    aLogSize,
                                     smlEnv    * aEnv )
{
    smfDatafilePersData sDatafilePersData;
    
    STL_WRITE_BYTES( &sDatafilePersData,
                     aLogBody,
                     STL_SIZEOF(smfDatafilePersData) );
    
    STL_TRY( smfDropDatafileUndo( smxmGetTransId( aMiniTrans ),
                                  aTargetRid.mTbsId,
                                  &sDatafilePersData,
                                  STL_TRUE, /* aClrLogging */
                                  aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfUndoMemoryTbsRename( smxmTrans * aMiniTrans,
                                  smlRid      aTargetRid,
                                  void      * aLogBody,
                                  stlInt16    aLogSize,
                                  smlEnv    * aEnv )
{
    STL_TRY( smfRenameTbsUndo( smxmGetTransId( aMiniTrans ),
                               aTargetRid.mTbsId,
                               (stlChar*)aLogBody,
                               aLogSize,
                               STL_TRUE, /* aClrLogging */
                               aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
