/*******************************************************************************
 * eldcUsagePrivDesc.h
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


#ifndef _ELDC_USAGE_PRIV_DESC_H_
#define _ELDC_USAGE_PRIV_DESC_H_ 1

/**
 * @file eldcUsagePrivDesc.h
 * @brief Cache for Usage Privilege descriptor
 */


/**
 * @defgroup eldcUsagePrivDesc Cache for Usage Privilege descriptor
 * @ingroup eldcPrivilege
 * @{
 */


/*
 * 초기화, 마무리 함수
 */

stlStatus eldcBuildUsagePrivCache( smlTransId     aTransID,
                                   smlStatement * aStmt,
                                   ellEnv       * aEnv );

/** @} eldcUsagePrivDesc */


#endif /* _ELDC_USAGE_PRIV_DESC_H_ */
