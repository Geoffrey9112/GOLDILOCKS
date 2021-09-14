/*******************************************************************************
 * eldcDatabasePrivDesc.h
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


#ifndef _ELDC_DATABASE_PRIV_DESC_H_
#define _ELDC_DATABASE_PRIV_DESC_H_ 1

/**
 * @file eldcDatabasePrivDesc.h
 * @brief Cache for Database Privilege descriptor
 */


/**
 * @defgroup eldcDatabasePrivDesc Cache for Database Privilege descriptor
 * @ingroup eldcPrivilege
 * @{
 */

/*
 * 초기화, 마무리 함수
 */

stlStatus eldcBuildDatabasePrivCache( smlTransId     aTransID,
                                      smlStatement * aStmt,
                                      ellEnv       * aEnv );


/** @} eldcDatabasePrivDesc */


#endif /* _ELDC_DATABASE_PRIV_DESC_H_ */
