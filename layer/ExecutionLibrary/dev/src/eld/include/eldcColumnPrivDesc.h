/*******************************************************************************
 * eldcColumnPrivDesc.h
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


#ifndef _ELDC_COLUMN_PRIV_DESC_H_
#define _ELDC_COLUMN_PRIV_DESC_H_ 1

/**
 * @file eldcColumnPrivDesc.h
 * @brief Cache for Column Privilege descriptor
 */


/**
 * @defgroup eldcColumnPrivDesc Cache for Column Privilege descriptor
 * @ingroup eldcPrivilege
 * @{
 */


/*
 * 초기화, 마무리 함수
 */

stlStatus eldcBuildColumnPrivCache( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    ellEnv       * aEnv );

/** @} eldcColumnPrivDesc */


#endif /* _ELDC_COLUMN_PRIV_DESC_H_ */
