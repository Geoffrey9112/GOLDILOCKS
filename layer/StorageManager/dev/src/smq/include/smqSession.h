/*******************************************************************************
 * smqSession.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smqSession.h 3137 2012-01-06 02:09:20Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMQSESSION_H_
#define _SMQSESSION_H_ 1

/**
 * @file smqSession.h
 * @brief Storage Manager Layer Sequence Session Internal Routines
 */

/**
 * @defgroup smqSession Sequence Session
 * @ingroup smqInternal
 * @{
 */

stlStatus smqSetSessSeqValue( smqCache * aCache,
                              stlInt64   aSeqValue,
                              smlEnv   * aEnv );

stlStatus smqRemoveSessSeqValue( smqCache * aCache,
                                 smlEnv   * aEnv );

stlStatus smqFindSessSeqValue( smqCache * aCache,
                               stlBool  * aFound,
                               stlInt64 * aSeqValue,
                               smlEnv   * aEnv );

/** @} */
    
#endif /* _SMQSESSION_H_ */
