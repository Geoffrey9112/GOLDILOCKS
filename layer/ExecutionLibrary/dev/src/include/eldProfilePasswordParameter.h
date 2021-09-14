/*******************************************************************************
 * eldProfilePasswordParameter.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldProfilePasswordParameter.h 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ELD_PROFILE_PASSWORD_PARAMETER_H_
#define _ELD_PROFILE_PASSWORD_PARAMETER_H_ 1

/**
 * @file eldProfilePasswordParameter.h
 * @brief ProfilePasswordParameter Dictionary 관리 루틴 
 */


/**
 * @defgroup eldProfilePasswordParameter ProfilePasswordParameter Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertProfilePassParamDesc( smlTransId             aTransID,
                                         smlStatement         * aStmt,
                                         stlInt64               aProfileID,
                                         ellProfileParam        aProfileParam,
                                         stlBool                aIsDefault,
                                         dtlDataValue         * aLimitValue,
                                         stlChar              * aLimitString,
                                         ellEnv               * aEnv );

stlStatus eldDeleteProfilePassParam( smlTransId             aTransID,
                                     smlStatement         * aStmt,
                                     stlInt64               aProfileID,
                                     ellProfileParam        aProfileParam,
                                     ellEnv               * aEnv );


/** @} eldProfilePasswordParameter */

#endif /* _ELD_PROFILE_H_ */
