/*******************************************************************************
 * smfmDatafile.h
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


#ifndef _SMFMDATAFILE_H_
#define _SMFMDATAFILE_H_ 1

/**
 * @file smfmDatafile.h
 * @brief Storage Manager Layer Memory Data File Component Internal Routines
 */

/**
 * @defgroup smfmDatafile Memory Datafile
 * @ingroup smfInternal
 * @{
 */

stlStatus smfmCreate( smxlTransId        aTransId,
                      stlInt32           aTbsPhysicalId,
                      stlInt32           aHintTbsId,
                      smfTbsInfo       * aTbsInfo,
                      smlDatafileAttr  * aDatafileAttr,
                      stlBool            aUndoLogging,
                      smfDatafileInfo ** aDatafileInfo,
                      smlEnv           * aEnv );
stlStatus smfmPrepare( smfTbsInfo       * aTbsInfo,
                       stlFile          * aFile,
                       smfCtrlBuffer    * aBuffer,
                       smfDatafileInfo ** aDatafileInfo,
                       smlEnv           * aEnv );
stlStatus smfmLoad( smfTbsInfo      * aTbsInfo,
                    smfDatafileInfo * aDatafileInfo,
                    smlEnv          * aEnv );
stlStatus smfmSave( stlFile         * aFile,
                    smfDatafileInfo * aDatafileInfo,
                    smlEnv          * aEnv );
stlStatus smfmDrop( smxlTransId       aTransId,
                    smfTbsInfo      * aTbsInfo,
                    smfDatafileInfo * aDatafileInfo,
                    smlEnv          * aEnv );
stlStatus smfmExtend( smxmTrans        *aMiniTrans,
                      smlTbsId          aTbsId,
                      smlDatafileId     aDatafileId,
                      smlEnv           *aEnv );
stlStatus smfmWritePage( smfTbsInfo      * aTbsInfo,
                         smfDatafileInfo * aDatafileInfo,
                         stlUInt32         aPageSeqID,
                         stlInt32          aPageCount,
                         void            * aPage,
                         smlEnv          * aEnv );

stlStatus smfmFillDatafileAttr( smfTbsInfo      * aTbsInfo,
                                smlDatafileAttr * aOrgDatafileAttr,
                                smlDatafileAttr * aRefinedDatafileAttr,
                                smlEnv          * aEnv );

/** @} */
    
#endif /* _SMFMDATAFILE_H_ */
