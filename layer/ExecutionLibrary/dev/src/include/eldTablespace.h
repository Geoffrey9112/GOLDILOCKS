/*******************************************************************************
 * eldTablespace.h
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


#ifndef _ELD_TABLESPACE_H_
#define _ELD_TABLESPACE_H_ 1

/**
 * @file eldTablespace.h
 * @brief Tablespace Dictionary 관리 루틴 
 */


/**
 * @defgroup eldTablespace Tablespace Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */


/*
 * Dictionary Row 변경 함수 
 */

stlStatus eldInsertTablespaceDesc( smlTransId               aTransID,
                                   smlStatement           * aStmt,
                                   stlInt64                 aTablespaceID,
                                   stlInt64                 aCreatorID,
                                   stlChar                * aTablespaceName,
                                   ellTablespaceMediaType   aMediaType,
                                   ellTablespaceUsageType   aUsageType,
                                   stlChar                * aTablespaceComment,
                                   ellEnv                 * aEnv );

stlStatus eldDeleteDictSpace( smlTransId       aTransID,
                              smlStatement   * aStmt,
                              ellDictHandle  * aSpaceHandle,
                              ellEnv         * aEnv );

/*
 * Dictionary Cache 변경 함수 
 */


stlStatus eldRefineCache4AddTablespace( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        stlInt64             aAuthID,
                                        stlInt64             aTablespaceID,
                                        ellEnv             * aEnv );

/*
 * Handle 획득 함수 
 */

stlStatus eldGetTablespaceHandleByID( smlTransId           aTransID,
                                      smlScn               aViewSCN,
                                      stlInt64             aTablespaceID,
                                      ellDictHandle      * aSpaceDictHandle,
                                      stlBool            * aExist,
                                      ellEnv             * aEnv );

stlStatus eldGetTablespaceHandleByName( smlTransId           aTransID,
                                        smlScn               aViewSCN,
                                        stlChar            * aTablespaceName,
                                        ellDictHandle      * aSpaceDictHandle,
                                        stlBool            * aExist,
                                        ellEnv             * aEnv );


/** @} eldTablespace */

#endif /* _ELD_TABLESPACE_H_ */
