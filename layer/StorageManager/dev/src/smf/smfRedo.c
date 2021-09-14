/*******************************************************************************
 * smfRedo.c
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
#include <smfRedo.h>
#include <smfRecovery.h>

extern smfDatafileFuncs * gDatafileFuncs[];

/**
 * @file smfRedo.c
 * @brief Storage Manager Datafile Redo Internal Routines
 */

/**
 * @addtogroup smfRedo
 * @{
 */

smrRedoVector gSmfRedoVectors[SMR_DATAFILE_LOG_MAX_COUNT] =
{
    { /* SMR_LOG_MEMORY_FILE_CREATE */
        smfRedoMemoryFileCreate,
        NULL,
        "MEMORY_FILE_CREATE"
    },
    { /* SMR_LOG_MEMORY_FILE_DROP */
        smfRedoMemoryFileDrop,
        NULL,
        "MEMORY_FILE_DROP"
    },
    { /* SMR_LOG_MEMORY_TBS_CREATE */
        smfRedoMemoryTbsCreate,
        NULL,
        "MEMORY_TBS_CREATE"
    },
    { /* SMR_LOG_MEMORY_TBS_CREATE_CLR */
        smfRedoMemoryTbsCreateClr,
        NULL,
        "MEMORY_TBS_CREATE_CLR"
    },
    { /* SMR_LOG_MEMORY_TBS_DROP */
        smfRedoMemoryTbsDrop,
        NULL,
        "MEMORY_TBS_DROP"
    },
    { /* SMR_LOG_MEMORY_TBS_DROP_CLR */
        smfRedoMemoryTbsDropClr,
        NULL,
        "MEMORY_TBS_DROP_CLR"
    },
    { /* SMR_LOG_MEMORY_DATAFILE_ADD */
        smfRedoMemoryDatafileAdd,
        NULL,
        "MEMORY_DATAFILE_ADD"
    },
    { /* SMR_LOG_MEMORY_DATAFILE_ADD_CLR */
        smfRedoMemoryDatafileAddClr,
        NULL,
        "MEMORY_DATAFILE_ADD_CLR"
    },
    { /* SMR_LOG_MEMORY_DATAFILE_DROP */
        smfRedoMemoryDatafileDrop,
        NULL,
        "MEMORY_DATAFILE_DROP"
    },
    { /* SMR_LOG_MEMORY_DATAFILE_DROP_CLR */
        smfRedoMemoryDatafileDropClr,
        NULL,
        "MEMORY_DATAFILE_DROP_CLR"
    },
    { /* SMR_LOG_MEMORY_TBS_RENAME */
        smfRedoMemoryTbsRename,
        NULL,
        "MEMORY_TBS_RENAME"
    },
    { /* SMR_LOG_MEMORY_TBS_RENAME_CLR */
        smfRedoMemoryTbsRenameClr,
        NULL,
        "MEMORY_TBS_RENAME_CLR"
    },
    { /* SMR_LOG_MEMORY_FILE_CREATE_CLR */
        smfRedoMemoryFileCreateClr,
        NULL,
        "MEMORY_FILE_CREATE_CLR"
    }
};

stlStatus smfRedoMemoryFileCreate( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv )
{
    stlInt32             sTbsTypeId;
    stlInt32             sTbsPhysicalId;
    stlInt32             sHintTbsId;
    stlInt32             sLogOffset = 0;
    smfDatafilePersData  sDatafilePersData;

    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( SML_TBS_DEVICE_MEMORY );

    STL_READ_MOVE_INT32( &sTbsPhysicalId, aData, sLogOffset );
    STL_READ_MOVE_INT32( &sHintTbsId, aData, sLogOffset );
    stlMemcpy( &sDatafilePersData,
               aData + sLogOffset,
               STL_SIZEOF(smfDatafilePersData) );
    
    STL_TRY( gDatafileFuncs[sTbsTypeId]->mCreateRedo( SML_INVALID_TRANSID,
                                                      sTbsPhysicalId,
                                                      sHintTbsId,
                                                      &sDatafilePersData,
                                                      STL_FALSE, /* For Restore */
                                                      STL_FALSE, /* aClrLogging */  
                                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfRedoMemoryFileCreateClr( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDatasize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv )
{
    stlInt32             sTbsTypeId;
    stlInt32             sTbsPhysicalId;
    stlInt32             sHintTbsId;
    stlInt32             sLogOffset = 0;
    smfDatafilePersData  sDatafilePersData;

    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( SML_TBS_DEVICE_MEMORY );

    STL_READ_MOVE_INT32( &sTbsPhysicalId, aData, sLogOffset );
    STL_READ_MOVE_INT32( &sHintTbsId, aData, sLogOffset );
    stlMemcpy( &sDatafilePersData,
               aData + sLogOffset,
               STL_SIZEOF(smfDatafilePersData) );

    STL_TRY( gDatafileFuncs[sTbsTypeId]->mCreateUndo( SML_INVALID_TRANSID,
                                                      sTbsPhysicalId,
                                                      sHintTbsId,
                                                      &sDatafilePersData,
                                                      STL_FALSE, /* For Restore */
                                                      STL_FALSE, /* aClrLogging */  
                                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfRedoMemoryFileDrop( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv )
{
    stlInt32             sTbsTypeId;
    stlInt32             sTbsPhysicalId;
    stlInt32             sLogOffset = 0;
    smfDatafilePersData  sDatafilePersData;

    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( SML_TBS_DEVICE_MEMORY );

    STL_READ_MOVE_INT32( &sTbsPhysicalId, aData, sLogOffset );
    stlMemcpy( &sDatafilePersData,
               aData + sLogOffset,
               STL_SIZEOF(smfDatafilePersData) );
    
    STL_TRY( gDatafileFuncs[sTbsTypeId]->mDropRedo( SML_INVALID_TRANSID,
                                                    sTbsPhysicalId,
                                                    SMF_INVALID_PHYSICAL_TBS_ID, /* aHintTbsId */
                                                    &sDatafilePersData,
                                                    STL_FALSE,  /* For Restore */
                                                    STL_FALSE, /* aClrLogging */  
                                                    aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfRedoMemoryTbsCreate( smlRid    * aDataRid,
                                  void      * aData,
                                  stlUInt16   aDataSize,
                                  smpHandle * aPageHandle,
                                  smlEnv    * aEnv )
{
    STL_TRY( smfCreateTbsRedo( (smfTbsPersData*)aData,
                               aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfRedoMemoryTbsCreateClr( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv )
{
    stlInt32 sTbsTypeId;

    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( SML_TBS_DEVICE_MEMORY );
    
    STL_TRY( smfCreateTbsUndo( SML_INVALID_TRANSID,
                               sTbsTypeId,
                               (smfTbsPersData*)aData,
                               STL_FALSE, /* aClrLogging */
                               aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfRedoMemoryTbsDrop( smlRid    * aDataRid,
                                void      * aData,
                                stlUInt16   aDataSize,
                                smpHandle * aPageHandle,
                                smlEnv    * aEnv )
{
    STL_TRY( smfDropTbsRedo( (smfTbsPersData*)aData,
                             aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfRedoMemoryTbsDropClr( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv )
{
    STL_TRY( smfDropTbsUndo( SML_INVALID_TRANSID,
                             (smfTbsPersData*)aData,
                             STL_FALSE, /* aClrLogging */
                             aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfRedoMemoryDatafileAdd( smlRid    * aDataRid,
                                    void      * aData,
                                    stlUInt16   aDataSize,
                                    smpHandle * aPageHandle,
                                    smlEnv    * aEnv )
{
    STL_TRY( smfAddDatafileRedo( aDataRid->mTbsId,
                                 (smfDatafilePersData*)aData,
                                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfRedoMemoryDatafileAddClr( smlRid    * aDataRid,
                                       void      * aData,
                                       stlUInt16   aDataSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv )
{
    stlInt32 sTbsTypeId;

    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( SML_TBS_DEVICE_MEMORY );
    
    STL_TRY( smfAddDatafileUndo( SML_INVALID_TRANSID,
                                 sTbsTypeId,
                                 aDataRid->mTbsId,
                                 (smfDatafilePersData*)aData,
                                 STL_FALSE, /* aClrLogging */
                                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfRedoMemoryDatafileDrop( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv )
{
    STL_TRY( smfDropDatafileRedo( aDataRid->mTbsId,
                                  (smfDatafilePersData*)aData,
                                  aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfRedoMemoryDatafileDropClr( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    STL_TRY( smfDropDatafileUndo( SML_INVALID_TRANSID,
                                  aDataRid->mTbsId,
                                  (smfDatafilePersData*)aData,
                                  STL_FALSE, /* aClrLogging */
                                  aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfRedoMemoryTbsRename( smlRid    * aDataRid,
                                  void      * aData,
                                  stlUInt16   aDataSize,
                                  smpHandle * aPageHandle,
                                  smlEnv    * aEnv )
{
    STL_TRY( smfRenameTbsRedo( aDataRid->mTbsId,
                               (stlChar*)aData,
                               aDataSize,
                               aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smfRedoMemoryTbsRenameClr( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv )
{
    STL_TRY( smfRenameTbsUndo( SML_INVALID_TRANSID,
                               aDataRid->mTbsId,
                               (stlChar*)aData,
                               aDataSize,
                               STL_FALSE, /* aClrLogging */
                               aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
