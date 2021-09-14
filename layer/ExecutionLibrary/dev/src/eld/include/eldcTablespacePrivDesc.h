/*******************************************************************************
 * eldcTablespacePrivDesc.h
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


#ifndef _ELDC_TABLESPACE_PRIV_DESC_H_
#define _ELDC_TABLESPACE_PRIV_DESC_H_ 1

/**
 * @file eldcTablespacePrivDesc.h
 * @brief Cache for Tablespace Privilege descriptor
 */


/**
 * @defgroup eldcTablespacePrivDesc Cache for Tablespace Privilege descriptor
 * @ingroup eldcPrivilege
 * @{
 */




/*
 * 초기화, 마무리 함수
 */

stlStatus eldcBuildSpacePrivCache( smlTransId     aTransID,
                                   smlStatement * aStmt,
                                   ellEnv       * aEnv );


/** @} eldcTablespacePrivDesc */


#endif /* _ELDC_TABLESPACE_PRIV_DESC_H_ */
