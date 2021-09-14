/*******************************************************************************
 * eldcTablePrivDesc.h
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


#ifndef _ELDC_TABLE_PRIV_DESC_H_
#define _ELDC_TABLE_PRIV_DESC_H_ 1

/**
 * @file eldcTablePrivDesc.h
 * @brief Cache for Table(View) Privilege descriptor
 */


/**
 * @defgroup eldcTablePrivDesc Cache for Table(View) Privilege descriptor
 * @ingroup eldcPrivilege
 * @{
 */


/*
 * 초기화, 마무리 함수
 */

stlStatus eldcBuildTablePrivCache( smlTransId     aTransID,
                                   smlStatement * aStmt,
                                   ellEnv       * aEnv );

/** @} eldcTablePrivDesc */


#endif /* _ELDC_TABLE_PRIV_DESC_H_ */
