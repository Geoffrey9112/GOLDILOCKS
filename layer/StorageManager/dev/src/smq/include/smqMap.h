/*******************************************************************************
 * smqMap.h
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


#ifndef _SMQMAP_H_
#define _SMQMAP_H_ 1

/**
 * @file smqMap.h
 * @brief Storage Manager Layer Sequence Map Internal Routines
 */

/**
 * @defgroup smqMap Sequence Map
 * @ingroup smqInternal
 * @{
 */

stlStatus smqInsertRecord( smlStatement * aStatement,
                           smqRecord    * aRecord,
                           smlRid       * aSequenceRid,
                           smlEnv       * aEnv );
stlStatus smqUpdateValue( smxmTrans  * aMiniTrans,
                          stlInt64     aSequenceValue,
                          smlRid     * aSequenceRid,
                          smlEnv     * aEnv );
stlStatus smqDeleteRecord( smlStatement * aStatement,
                           smlRid       * aSequenceRid,
                           smlEnv       * aEnv );

/** @} */
    
#endif /* _SMQMAP_H_ */
