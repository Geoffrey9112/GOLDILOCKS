/*******************************************************************************
 * eldcSchemaPrivDesc.h
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


#ifndef _ELDC_SCHEMA_PRIV_DESC_H_
#define _ELDC_SCHEMA_PRIV_DESC_H_ 1

/**
 * @file eldcSchemaPrivDesc.h
 * @brief Cache for Schema Privilege descriptor
 */


/**
 * @defgroup eldcSchemaPrivDesc Cache for Schema Privilege descriptor
 * @ingroup eldcPrivilege
 * @{
 */


/*
 * 초기화, 마무리 함수
 */

stlStatus eldcBuildSchemaPrivCache( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    ellEnv       * aEnv );

/** @} eldcSchemaPrivDesc */


#endif /* _ELDC_SCHEMA_PRIV_DESC_H_ */
