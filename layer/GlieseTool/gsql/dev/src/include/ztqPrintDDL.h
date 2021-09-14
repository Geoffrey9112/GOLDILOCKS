/*******************************************************************************
 * ztqPrintDDL.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztqPrintDDL.h 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ZTQ_PRINT_DDL_H_
#define _ZTQ_PRINT_DDL_H_ 1

/**
 * @file ztqPrintDDL.h
 * @brief Print DDL SQL String
 */

/**
 * @defgroup ztqPrintDDL Print SQL-Object Print DDL
 * @ingroup ztq
 * @{
 */

#define ZTQ_CONSTRANT_NOT_NULL    ( "NOT NULL" )
#define ZTQ_CONSTRANT_PRIMARY_KEY ( "PRIMARY KEY" )
#define ZTQ_CONSTRANT_UNIQUE      ( "UNIQUE" )
#define ZTQ_MAX_COMMENT_LENGTH    ( 1024 + 1 )
#define ZTQ_SYSTEM_USER_NAME      ( "_SYSTEM" )
#define ZTQ_COLUMNAR_TABLE_TYPE   ( "HEAP_COLUMNAR" )
#define ZTQ_UNKNOWN_PASSWORD      ( "GOLDILOCKS" )


#define ZTQ_PRIV_ORDER_NA         ( -1 )

typedef enum
{
    ZTQ_PRIV_OBJECT_DATABASE = 0,
    
    ZTQ_PRIV_OBJECT_TABLESPACE,
    ZTQ_PRIV_OBJECT_SCHEMA,

    ZTQ_PRIV_OBJECT_TABLE,
    ZTQ_PRIV_OBJECT_COLUMN,
    ZTQ_PRIV_OBJECT_SEQUENCE,

    ZTQ_PRIV_MAX
    
} ztqPrivObjectType;
    
typedef struct ztqPrivItem
{
    stlInt64  mGrantorID;
    stlInt64  mGranteeID;

    stlChar   mGrantorName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar   mGranteeName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    stlBool   mIsBuiltIn;
    stlBool   mIsGrantable;
    
    struct ztqPrivItem  * mNext;       /**< Priv Item 의 추가 목록 */
    
    stlInt32              mPrivOrder;
    struct ztqPrivItem  * mNextOrder;  /**< Priv Item 의 순서 목록 */
} ztqPrivItem;

typedef struct ztqPrivOrder
{
    stlInt32       mPrivCount;
    ztqPrivItem  * mHeadItem;
    ztqPrivItem  * mTailItem;

    ztqPrivItem  * mHeadOrder;
    ztqPrivItem  * mTailOrder;
} ztqPrivOrder;

/****************************************************************************
 * Common
 ****************************************************************************/

stlStatus ztqFinishPrintDDL( stlChar       * aStringDDL,
                             stlErrorStack * aErrorStack );

stlStatus ztqFinishPrintComment( stlChar       * aStringDDL,
                                 stlErrorStack * aErrorStack );

void ztqInitPrivOrder( ztqPrivOrder * aPrivOrder );

stlStatus ztqAddPrivItem( ztqPrivOrder  * aPrivOrder,
                          stlInt64        aGrantorID,
                          stlChar       * aGrantorName,
                          stlInt64        aGranteeID,
                          stlChar       * aGranteeName,
                          stlChar       * aIsBuiltInPriv,
                          stlChar       * aIsGrantable,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack );

void ztqBuildPrivOrder( ztqPrivOrder  * aPrivOrder, stlInt64        aOwnerID );

stlStatus ztqPrintGrantByPrivOrder( stlChar           * aStringDDL,
                                    ztqPrivOrder      * aPrivOrder,
                                    ztqPrivObjectType   aObjectType,
                                    stlChar           * aPrivName,
                                    stlChar           * aNonSchemaName,
                                    stlChar           * aObjectName,
                                    stlChar           * aColumnName,
                                    stlErrorStack     * aErrorStack );


/****************************************************************************
 * DATABASE 
 ****************************************************************************/

stlStatus ztqPrintAllDDL( stlChar       * aStringDDL,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack );

stlStatus ztqPrintAllSpaceDDL( stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqPrintAllSpaceGRANT( stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

stlStatus ztqPrintAllProfileDDL( stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

stlStatus ztqPrintAllUserDDL( stlChar       * aStringDDL,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack );

stlStatus ztqPrintAllUserSchemaPathDDL( stlChar       * aStringDDL,
                                        stlAllocator  * aAllocator,
                                        stlErrorStack * aErrorStack );

stlStatus ztqPrintAllSchemaDDL( stlChar       * aStringDDL,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack );

stlStatus ztqPrintAllSchemaGRANT( stlChar       * aStringDDL,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack );

stlStatus ztqPrintAllTableDDL( stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqPrintAllTableGRANT( stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

stlStatus ztqPrintAllTableOptionDDL( stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack );

stlStatus ztqPrintAllConstraintDDL( stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack );

stlStatus ztqPrintAllIndexDDL( stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqPrintAllViewDDL( stlChar       * aStringDDL,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack );

stlStatus ztqPrintAllViewGRANT( stlChar       * aStringDDL,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack );

stlStatus ztqPrintAllSequenceDDL( stlChar       * aStringDDL,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack );

stlStatus ztqPrintAllSequenceGRANT( stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack );

stlStatus ztqPrintAllSynonymDDL( stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

stlStatus ztqPrintAllPublicSynonymDDL( stlChar       * aStringDDL,
                                       stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack );

stlStatus ztqPrintGrantDB( stlChar       * aStringDDL,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack );

stlStatus ztqPrintCommentDB( stlChar       * aStringDDL,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

/****************************************************************************
 * Tablespace
 ****************************************************************************/

stlStatus ztqGetSpaceID( ztqIdentifier * aIdentifier,
                         stlAllocator  * aAllocator,
                         stlInt64      * aSpaceID,
                         stlBool       * aIsBuiltIn,
                         stlErrorStack * aErrorStack );

stlStatus ztqPrintCreateSpace( stlInt64        aSpaceID,
                               stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqPrintAlterSpace( stlInt64        aSpaceID,
                              stlChar       * aStringDDL,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack );

stlStatus ztqPrintSpaceCreateTable( stlInt64        aSpaceID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack );

stlStatus ztqPrintSpaceConstraint( stlInt64        aSpaceID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack );

stlStatus ztqPrintSpaceCreateIndex( stlInt64        aSpaceID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack );

stlStatus ztqPrintGrantSpace( stlInt64        aSpaceID,
                              stlChar       * aStringDDL,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack );

stlStatus ztqPrintCommentSpace( stlInt64        aSpaceID,
                                stlChar       * aStringDDL,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack );

/****************************************************************************
 * Profile
 ****************************************************************************/

stlStatus ztqGetProfileID( ztqIdentifier * aIdentifier,
                           stlAllocator  * aAllocator,
                           stlInt64      * aSpaceID,
                           stlErrorStack * aErrorStack );

stlStatus ztqPrintCreateProfile( stlInt64        aProfileID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

stlStatus ztqPrintCommentProfile( stlInt64        aProfileID,
                                  stlChar       * aStringDDL,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack );

/****************************************************************************
 * Authorization
 ****************************************************************************/

stlStatus ztqGetAuthID( ztqIdentifier * aIdentifier,
                        stlAllocator  * aAllocator,
                        stlInt64      * aAuthID,
                        stlBool       * aIsUser,
                        stlErrorStack * aErrorStack );

stlStatus ztqPrintCreateUser( stlInt64        aUserID,
                              stlChar       * aStringDDL,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack );

stlStatus ztqPrintAlterUserSchemaPath( stlInt64        aUserID,
                                       stlChar       * aStringDDL,
                                       stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack );

stlStatus ztqPrintOwnerCreateSchema( stlInt64        aOwnerID,
                                     stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack );

stlStatus ztqPrintOwnerCreateTable( stlInt64        aOwnerID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack );

stlStatus ztqPrintOwnerConstraint( stlInt64        aOwnerID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack );

stlStatus ztqPrintOwnerCreateIndex( stlInt64        aOwnerID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack );

stlStatus ztqPrintOwnerCreateView( stlInt64        aOwnerID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack );

stlStatus ztqPrintOwnerCreateSequence( stlInt64        aOwnerID,
                                       stlChar       * aStringDDL,
                                       stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack );

stlStatus ztqPrintOwnerCreateSynonym( stlInt64        aOwnerID,
                                      stlChar       * aStringDDL,
                                      stlAllocator  * aAllocator,
                                      stlErrorStack * aErrorStack );

stlStatus ztqPrintCommentAuth( stlInt64        aAuthID,
                               stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

/****************************************************************************
 * Schema
 ****************************************************************************/

stlStatus ztqGetSchemaID( ztqIdentifier * aIdentifier,
                          stlAllocator  * aAllocator,
                          stlInt64      * aSchemaID,
                          stlErrorStack * aErrorStack );

stlStatus ztqPrintCreateSchema( stlInt64        aSchemaID,
                                stlChar       * aStringDDL,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack );

stlStatus ztqPrintSchemaCreateTable( stlInt64        aSchemaID,
                                     stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack );

stlStatus ztqPrintSchemaConstraint( stlInt64        aSchemaID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack );

stlStatus ztqPrintSchemaCreateIndex( stlInt64        aSchemaID,
                                     stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack );

stlStatus ztqPrintSchemaCreateView( stlInt64        aSchemaID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack );

stlStatus ztqPrintSchemaCreateSequence( stlInt64        aSchemaID,
                                        stlChar       * aStringDDL,
                                        stlAllocator  * aAllocator,
                                        stlErrorStack * aErrorStack );

stlStatus ztqPrintSchemaCreateSynonym( stlInt64        aSchemaID,
                                       stlChar       * aStringDDL,
                                       stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack );

stlStatus ztqPrintGrantSchema( stlInt64        aSchemaID,
                               stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqPrintCommentSchema( stlInt64        aSchemaID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

/****************************************************************************
 * Public Synonym
 ****************************************************************************/

stlStatus ztqGetPublicSynonymID( ztqIdentifier * aIdentifier,
                                 stlAllocator  * aAllocator,
                                 stlInt64      * aSynonymID,
                                 stlErrorStack * aErrorStack );

stlStatus ztqGetQualifiedPublicSynonymID( stlChar       * aSynonymName,
                                          stlAllocator  * aAllocator,
                                          stlInt64      * aSynonymID,
                                          stlErrorStack * aErrorStack );

stlStatus ztqGetNonQualifiedPublicSynonymID( stlChar       * aSynonymName,
                                             stlAllocator  * aAllocator,
                                             stlInt64      * aSynonymID,
                                             stlErrorStack * aErrorStack );

stlStatus ztqPrintCreatePublicSynonym( stlInt64        aSynonymID,
                                       stlChar       * aStringDDL,
                                       stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack );

/****************************************************************************
 * Table
 ****************************************************************************/

stlStatus ztqGetTableID( ztqIdentifier * aIdentifierList,
                         stlAllocator  * aAllocator,
                         stlInt64      * aTableID,
                         stlErrorStack * aErrorStack );

stlStatus ztqGetQualifiedTableID( stlChar       * aSchemaName,
                                  stlChar       * aTableName,
                                  stlAllocator  * aAllocator,
                                  stlInt64      * aTableID,
                                  stlErrorStack * aErrorStack );

stlStatus ztqGetNonQualifiedTableID( stlChar       * aTableName,
                                     stlAllocator  * aAllocator,
                                     stlInt64      * aTableID,
                                     stlErrorStack * aErrorStack );

stlStatus ztqPrintCreateTable( stlInt64        aTableID,
                               stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqPrintTableConstraint( stlInt64        aTableID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack );

stlStatus ztqPrintTableCreateIndex( stlInt64        aTableID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack );

stlStatus ztqPrintTableIdentity( stlInt64        aTableID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

stlStatus ztqPrintTableSupplemental( stlInt64        aTableID,
                                     stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack );


stlStatus ztqPrintGrantRelation( stlInt64        aRelationID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

stlStatus ztqPrintCommentRelation( stlInt64        aRelationID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack );

/****************************************************************************
 * Constraint
 ****************************************************************************/

stlStatus ztqGetConstraintID( ztqIdentifier * aIdentifierList,
                              stlAllocator  * aAllocator,
                              stlInt64      * aConstID,
                              stlErrorStack * aErrorStack );

stlStatus ztqGetQualifiedConstraintID( stlChar       * aSchemaName,
                                       stlChar       * aConstName,
                                       stlAllocator  * aAllocator,
                                       stlInt64      * aConstID,
                                       stlErrorStack * aErrorStack );

stlStatus ztqGetNonQualifiedConstraintID( stlChar       * aConstName,
                                          stlAllocator  * aAllocator,
                                          stlInt64      * aConstID,
                                          stlErrorStack * aErrorStack );

stlStatus ztqPrintAddConstraint( stlInt64        aConstID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

stlStatus ztqPrintNotNullConstraint( stlInt64        aConstID,
                                     stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack );

stlStatus ztqPrintUniquePrimaryConstraint( stlInt64        aConstID,
                                           stlChar       * aStringDDL,
                                           stlAllocator  * aAllocator,
                                           stlErrorStack * aErrorStack );

stlStatus ztqPrintCommentConstraint( stlInt64        aConstID,
                                     stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack );


/****************************************************************************
 * Index
 ****************************************************************************/

stlStatus ztqGetIndexID( ztqIdentifier * aIdentifierList,
                         stlAllocator  * aAllocator,
                         stlInt64      * aIndexID,
                         stlErrorStack * aErrorStack );

stlStatus ztqGetQualifiedIndexID( stlChar       * aSchemaName,
                                  stlChar       * aIndexName,
                                  stlAllocator  * aAllocator,
                                  stlInt64      * aIndexID,
                                  stlErrorStack * aErrorStack );

stlStatus ztqGetNonQualifiedIndexID( stlChar       * aIndexName,
                                     stlAllocator  * aAllocator,
                                     stlInt64      * aIndexID,
                                     stlErrorStack * aErrorStack );

stlStatus ztqPrintCreateIndex( stlInt64        aIndexID,
                               stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack );

stlStatus ztqPrintCommentIndex( stlInt64        aIndexID,
                                stlChar       * aStringDDL,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack );

/****************************************************************************
 * View
 ****************************************************************************/

stlStatus ztqGetViewID( ztqIdentifier * aIdentifierList,
                        stlAllocator  * aAllocator,
                        stlInt64      * aViewID,
                        stlErrorStack * aErrorStack );

stlStatus ztqGetQualifiedViewID( stlChar       * aSchemaName,
                                 stlChar       * aTableName,
                                 stlAllocator  * aAllocator,
                                 stlInt64      * aViewID,
                                 stlErrorStack * aErrorStack );

stlStatus ztqGetNonQualifiedViewID( stlChar       * aTableName,
                                    stlAllocator  * aAllocator,
                                    stlInt64      * aViewID,
                                    stlErrorStack * aErrorStack );

stlStatus ztqPrintCreateView( stlInt64        aViewID,
                              stlChar       * aStringDDL,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack );

stlStatus ztqPrintViewRecompile( stlInt64        aViewID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

/****************************************************************************
 * Sequence
 ****************************************************************************/

stlStatus ztqGetSequenceID( ztqIdentifier * aIdentifierList,
                            stlAllocator  * aAllocator,
                            stlInt64      * aSeqID,
                            stlErrorStack * aErrorStack );

stlStatus ztqGetQualifiedSequenceID( stlChar       * aSchemaName,
                                     stlChar       * aSeqName,
                                     stlAllocator  * aAllocator,
                                     stlInt64      * aSeqID,
                                     stlErrorStack * aErrorStack );

stlStatus ztqGetNonQualifiedSequenceID( stlChar       * aSeqName,
                                        stlAllocator  * aAllocator,
                                        stlInt64      * aSeqID,
                                        stlErrorStack * aErrorStack );

stlStatus ztqPrintCreateSequence( stlInt64        aSeqID,
                                  stlChar       * aStringDDL,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack );

stlStatus ztqPrintRestartSequence( stlInt64        aSeqID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack );

stlStatus ztqPrintGrantSequence( stlInt64        aSeqID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

stlStatus ztqPrintCommentSequence( stlInt64        aSeqID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack );

/****************************************************************************
 * Synonym
 ****************************************************************************/

stlStatus ztqGetSynonymID( ztqIdentifier * aIdentifierList,
                           stlAllocator  * aAllocator,
                           stlInt64      * aSynonymID,
                           stlErrorStack * aErrorStack );

stlStatus ztqGetQualifiedSynonymID( stlChar       * aSchemaName,
                                    stlChar       * aSynonymName,
                                    stlAllocator  * aAllocator,
                                    stlInt64      * aSynonymID,
                                    stlErrorStack * aErrorStack );

stlStatus ztqGetNonQualifiedSynonymID( stlChar       * aSynonymName,
                                       stlAllocator  * aAllocator,
                                       stlInt64      * aSynonymID,
                                       stlErrorStack * aErrorStack );

stlStatus ztqPrintCreateSynonym( stlInt64        aSynonymID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );


/** @} */

#endif /* _ZTQ_PRINT_DDL_H_ */
