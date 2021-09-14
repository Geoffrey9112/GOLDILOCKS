/*******************************************************************************
 * ztqDesc.h
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


#ifndef _ZTQDESC_H_
#define _ZTQDESC_H_ 1

/**
 * @file ztqDesc.h
 * @brief Show Table Description Definition
 */

/**
 * @defgroup ztqDesc Show Table Description
 * @ingroup ztq
 * @{
 */

stlStatus ztqExecuteTableDesc( ztqIdentifier * aIdentifierList,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqExecuteIndexDesc( ztqIdentifier * aIdentifierList,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqGetUserId( stlChar       * aUserName,
                        stlInt64      * aUserId,
                        stlAllocator  * aAllocator,
                        stlErrorStack * aErrorStack );

stlStatus ztqGetSchemaId( stlChar       * aSchemaName,
                          stlInt64      * aSchemaId,
                          stlBool       * aIsSuccess,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack );

stlStatus ztqGetTableId( stlChar       * aTableName,
                         stlInt64        aSchemaId,
                         stlInt64      * aTableId,
                         stlBool       * aIsSuccess,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack );

stlStatus ztqGetIndexId( stlChar       * aIndexName,
                         stlInt64        aSchemaId,
                         stlInt64      * aIndexId,
                         stlBool       * aIsSuccess,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack );

stlStatus ztqPrintTableDesc( stlChar       * aSchemaName,
                             stlChar       * aTableName,
                             stlChar       * aOutSchemaName,
                             stlChar       * aOutTableName,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

stlStatus ztqGetAccessableIndex( stlChar       * aInSchemaName,
                                 stlChar       * aInIndexName,
                                 stlChar       * aOutSchemaName,
                                 stlChar       * aOutIndexName,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

stlStatus ztqPrintIndexDesc( stlInt64        aIndexId,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

stlStatus ztqGetObjectIds( stlChar       * aSqlString,
                           stlInt64     ** aObjectIds,
                           stlInt32      * aObjectIdCount,
                           stlBool       * aIsSuccess,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack );

stlStatus ztqGetObjectName( stlChar       * aSqlString,
                            stlChar       * aObjectName,
                            stlBool       * aIsSuccess,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack );

stlStatus ztqGetIndexInfo( stlInt64        aIndexId,
                           stlChar       * aIndexName,
                           stlChar       * aTablespaceName,
                           stlChar       * aIndexTypeStr,
                           stlChar       * aIsUniqueStr,
                           stlBool       * aIsSuccess,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack );

stlStatus ztqGetTablespaceName( stlInt64        aTablespaceId,
                                stlChar       * aTablespaceName,
                                stlBool       * aIsSuccess,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack );

stlStatus ztqGetColumnName( stlInt64        aColumnId,
                            stlChar       * aColumnName,
                            stlBool       * aIsSuccess,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack );

stlStatus ztqPrintIndexInfo( stlInt64        aSchemaId,
                             stlInt64        aTableId,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

stlStatus ztqGetIndexColumnNameList( stlInt64        aIndexId,
                                     stlChar       * aIndexNameList,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack );

stlStatus ztqGetKeyConstColumnNameList( stlInt64        aConstId,
                                        stlChar       * aColumnNameList,
                                        stlAllocator  * aAllocator,
                                        stlErrorStack * aErrorStack );

stlStatus ztqGetCheckConstColumnNameList( stlInt64        aConstId,
                                          stlChar       * aColumnNameList,
                                          stlAllocator  * aAllocator,
                                          stlErrorStack * aErrorStack );

stlStatus ztqPrintConstraintInfo( stlInt64        aSchemaId,
                                  stlInt64        aTableId,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack );

stlStatus ztqGetConstraintInfo( stlInt64        aConstId,
                                stlChar       * aConstName,
                                stlChar       * aConstTypeStr,
                                stlBool       * aIsKeyConst,
                                stlChar       * aAssociatedIndexName,
                                stlBool       * aIsSuccess,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZTQDESC_H_ */
