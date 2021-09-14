/*******************************************************************************
 * eldProfile.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldProfile.h 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ELD_PROFILE_H_
#define _ELD_PROFILE_H_ 1

/**
 * @file eldProfile.h
 * @brief Profile Dictionary 관리 루틴 
 */


/**
 * @defgroup eldProfile Profile Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertProfileDesc( smlTransId             aTransID,
                                smlStatement         * aStmt,
                                stlInt64             * aProfileID,
                                stlChar              * aProfileName,
                                stlChar              * aProfileComment,
                                ellEnv               * aEnv );


/** @} eldProfile */

#endif /* _ELD_PROFILE_H_ */
