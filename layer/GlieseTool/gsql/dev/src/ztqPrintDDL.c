/*******************************************************************************
 * ztqPrintDDL.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztqPrintDDL.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztqDef.h>
#include <ztqHostVariable.h>
#include <ztqDesc.h>
#include <ztqDisplay.h>
#include <ztqOdbcBridge.h>
#include <ztqPrintDDL.h>


extern stlInt32 gZtqDdlSize;
extern stlBool  gZtqPrintResult;

extern SQLHDBC  gZtqDbcHandle;

/**
 * @file ztqPrintDDL.c
 * @brief Print DDL Routines
 */

/**
 * @addtogroup ztqPrintDDL
 * @{
 */

/****************************************************************************
 * Common
 ****************************************************************************/

stlStatus ztqFinishPrintDDL( stlChar       * aStringDDL,
                             stlErrorStack * aErrorStack )
{
    stlInt32  sLength = 0;
    
    /**
     * 구문 마무리
     */

    sLength = stlStrlen( aStringDDL );
    
    STL_TRY_THROW( (sLength + 3) < gZtqDdlSize, RAMP_ERR_NOT_ENOUGH_DDLSIZE );

    if ( gZtqPrintResult == STL_TRUE )
    {
        ztqPrintf( "%s"
                   "\n;\n"
                   "COMMIT;\n",
                   aStringDDL );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_ENOUGH_DDLSIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_ENOUGH_DDLSIZE,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;

}


stlStatus ztqFinishPrintComment( stlChar       * aStringDDL,
                                 stlErrorStack * aErrorStack )
{
    stlInt32  sLength = 0;
    
    /**
     * 구문 마무리
     */

    sLength = stlStrlen( aStringDDL );
    
    STL_TRY_THROW( (sLength + 2) < gZtqDdlSize, RAMP_ERR_NOT_ENOUGH_DDLSIZE );

    if ( gZtqPrintResult == STL_TRUE )
    {
        ztqPrintf( "%s"
                   "\n\n",
                   aStringDDL );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_ENOUGH_DDLSIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_ENOUGH_DDLSIZE,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;

}

void ztqInitPrivOrder( ztqPrivOrder * aPrivOrder )
{
    aPrivOrder->mPrivCount = 0;
    aPrivOrder->mHeadItem  = NULL;
    aPrivOrder->mTailItem  = NULL;

    aPrivOrder->mHeadOrder = NULL;
    aPrivOrder->mTailOrder = NULL;
}

stlStatus ztqAddPrivItem( ztqPrivOrder  * aPrivOrder,
                          stlInt64        aGrantorID,
                          stlChar       * aGrantorName,
                          stlInt64        aGranteeID,
                          stlChar       * aGranteeName,
                          stlChar       * aIsBuiltInPriv,
                          stlChar       * aIsGrantable,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack )
{
    ztqPrivItem * sItem = NULL;

    /**
     * Privilege Item 생성
     */
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF( ztqPrivItem ),
                                (void**) & sItem,
                                aErrorStack )
             == STL_SUCCESS );

    sItem->mGrantorID = aGrantorID;
    sItem->mGranteeID = aGranteeID;

    stlStrncpy( sItem->mGrantorName, aGrantorName, STL_MAX_SQL_IDENTIFIER_LENGTH );
    stlStrncpy( sItem->mGranteeName, aGranteeName, STL_MAX_SQL_IDENTIFIER_LENGTH );

    if ( stlStrcmp( aIsBuiltInPriv, "YES" ) == 0 )
    {
        sItem->mIsBuiltIn = STL_TRUE;
    }
    else
    {
        sItem->mIsBuiltIn = STL_FALSE;
    }

    if ( stlStrcmp( aIsGrantable, "YES" ) == 0 )
    {
        sItem->mIsGrantable = STL_TRUE;
    }
    else
    {
        sItem->mIsGrantable = STL_FALSE;
    }
    
    sItem->mNext = NULL;

    sItem->mPrivOrder = ZTQ_PRIV_ORDER_NA;
    sItem->mNextOrder = NULL;

    /**
     * Privilege Item 연결
     */
    
    if ( aPrivOrder->mPrivCount == 0 )
    {
        aPrivOrder->mHeadItem = sItem;
        aPrivOrder->mTailItem = sItem;
    }
    else
    {
        aPrivOrder->mTailItem->mNext = sItem;
        aPrivOrder->mTailItem = sItem;
    }

    aPrivOrder->mPrivCount++;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                          

void ztqBuildPrivOrder( ztqPrivOrder  * aPrivOrder, stlInt64        aOwnerID )
{
    ztqPrivItem * sChildItem = NULL;
    ztqPrivItem * sParentItem = NULL;
    
    stlInt32   sOrder = 0;
    stlInt32   sLoop = 0;

    
    /**
     * Owner 를 기준으로 Privilege Order 를 설정
     */

    sOrder = 0;
    for ( sChildItem = aPrivOrder->mHeadItem; sChildItem != NULL; sChildItem = sChildItem->mNext )
    {
        if ( sChildItem->mGrantorID == aOwnerID )
        {
            sChildItem->mPrivOrder = sOrder;
            sOrder++;

            if ( aPrivOrder->mTailOrder == NULL )
            {
                aPrivOrder->mHeadOrder = sChildItem;
                aPrivOrder->mTailOrder = sChildItem;
            }
            else
            {
                aPrivOrder->mTailOrder->mNextOrder = sChildItem;
                aPrivOrder->mTailOrder = sChildItem;
            }
        }
        else
        {
            /* pass */
        }
    }

    /**
     * Order 가 결정되지 않은 Privilege Item 의 Order 결정
     */

    sLoop  = 0;
    while ( sOrder < aPrivOrder->mPrivCount )
    {
        for ( sChildItem = aPrivOrder->mHeadItem; sChildItem != NULL; sChildItem = sChildItem->mNext )
        {
            if ( sChildItem->mPrivOrder == ZTQ_PRIV_ORDER_NA )
            {
                /**
                 * Order 가 결정되지 않은 Item 임
                 */
                
                for ( sParentItem = aPrivOrder->mHeadItem; sParentItem != NULL; sParentItem = sParentItem->mNext )
                {
                    if ( sChildItem->mGrantorID == sParentItem->mGranteeID )
                    {
                        /* child 의 부모임 */
                        if ( sParentItem->mPrivOrder == ZTQ_PRIV_ORDER_NA )
                        {
                            /* parent 의 order 가 결정되지 않음 */
                        }
                        else
                        {
                            /* parent 의 order 가 정해짐 */
                            sChildItem->mPrivOrder = sOrder;
                            sOrder++;
                            
                            if ( aPrivOrder->mTailOrder == NULL )
                            {
                                aPrivOrder->mHeadOrder = sChildItem;
                                aPrivOrder->mTailOrder = sChildItem;
                            }
                            else
                            {
                                aPrivOrder->mTailOrder->mNextOrder = sChildItem;
                                aPrivOrder->mTailOrder = sChildItem;
                            }
                            break;
                        }
                    }
                    else
                    {
                        /* child 의 부모가 아님 */
                    }
                }
            }
            else
            {
                /* 이미 order 가 결정됨 */
                continue;
            }
        }

        /**
         * Validation
         */
        
        if ( sLoop > (aPrivOrder->mPrivCount * aPrivOrder->mPrivCount) )
        {
            /**
             * 최대 Loop 횟수만큼 반복되면 Order 가 모두 결정되어 있어야 함
             * orphan 인 priv item 이 존재하는 경우임.
             */
            
            /* STL_DASSERT( 0 ); */

            /**
             * 무한 loop 를 방지하기 위해 orphan 에 강제로 order 를 부여함
             */

            for ( sChildItem = aPrivOrder->mHeadItem; sChildItem != NULL; sChildItem = sChildItem->mNext )
            {
                if ( sChildItem->mPrivOrder == ZTQ_PRIV_ORDER_NA )
                {
                    sChildItem->mPrivOrder = sOrder;
                    sOrder++;
                    
                    if ( aPrivOrder->mTailOrder == NULL )
                    {
                        aPrivOrder->mHeadOrder = sChildItem;
                        aPrivOrder->mTailOrder = sChildItem;
                    }
                    else
                    {
                        aPrivOrder->mTailOrder->mNextOrder = sChildItem;
                        aPrivOrder->mTailOrder = sChildItem;
                    }
                }
            }
        }
        else
        {
            sLoop++;
        }
    }
}
                             
stlStatus ztqPrintGrantByPrivOrder( stlChar           * aStringDDL,
                                    ztqPrivOrder      * aPrivOrder,
                                    ztqPrivObjectType   aObjectType,
                                    stlChar           * aPrivName,
                                    stlChar           * aNonSchemaName,
                                    stlChar           * aObjectName,
                                    stlChar           * aColumnName,
                                    stlErrorStack     * aErrorStack )
{
    ztqPrivItem * sItem = NULL;

    for ( sItem = aPrivOrder->mHeadOrder; sItem != NULL; sItem = sItem->mNextOrder )
    {
        /**
         * Built-In Privilege 는 출력하지 않는다.
         */
        
        if ( sItem->mIsBuiltIn == STL_TRUE )
        {
            continue;
        }
        
        /**
         * SET SESSION AUTHORIZATION grantor
         */

        if ( stlStrcmp( sItem->mGrantorName, ZTQ_SYSTEM_USER_NAME ) == 0 )
        {
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "\nSET SESSION AUTHORIZATION \"SYS\"; " );
        }
        else
        {
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "\nSET SESSION AUTHORIZATION \"%s\"; ",
                         sItem->mGrantorName );
        }

        /**
         * GRANT privilege
         */
        
        switch ( aObjectType )
        {
            case ZTQ_PRIV_OBJECT_DATABASE:
                {
                    STL_DASSERT( aPrivName != NULL );
                    STL_DASSERT( aNonSchemaName == NULL );
                    STL_DASSERT( aObjectName == NULL );
                    STL_DASSERT( aColumnName == NULL );
                    
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s"
                                 "\nGRANT "
                                 "\n    %s ON DATABASE "
                                 "\n    TO \"%s\" ",
                                 aStringDDL,
                                 aPrivName,
                                 sItem->mGranteeName );
                    break;
                }
            case ZTQ_PRIV_OBJECT_TABLESPACE:
                {
                    STL_DASSERT( aPrivName != NULL );
                    STL_DASSERT( aNonSchemaName != NULL );
                    STL_DASSERT( aObjectName == NULL );
                    STL_DASSERT( aColumnName == NULL );

                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s"
                                 "\nGRANT "
                                 "\n    %s ON TABLESPACE \"%s\" "
                                 "\n    TO \"%s\" ",
                                 aStringDDL,
                                 aPrivName,
                                 aNonSchemaName,
                                 sItem->mGranteeName );
                    
                    break;
                }
            case ZTQ_PRIV_OBJECT_SCHEMA:
                {
                    STL_DASSERT( aPrivName != NULL );
                    STL_DASSERT( aNonSchemaName != NULL );
                    STL_DASSERT( aObjectName == NULL );
                    STL_DASSERT( aColumnName == NULL );

                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s"
                                 "\nGRANT "
                                 "\n    %s ON SCHEMA \"%s\" "
                                 "\n    TO \"%s\" ",
                                 aStringDDL,
                                 aPrivName,
                                 aNonSchemaName,
                                 sItem->mGranteeName );
                    break;
                }

            case ZTQ_PRIV_OBJECT_TABLE:
                {
                    STL_DASSERT( aPrivName != NULL );
                    STL_DASSERT( aNonSchemaName != NULL );
                    STL_DASSERT( aObjectName != NULL );
                    STL_DASSERT( aColumnName == NULL );

                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s"
                                 "\nGRANT "
                                 "\n    %s ON TABLE \"%s\".\"%s\" "
                                 "\n    TO \"%s\" ",
                                 aStringDDL,
                                 aPrivName,
                                 aNonSchemaName,
                                 aObjectName,
                                 sItem->mGranteeName );
                    break;
                }
            case ZTQ_PRIV_OBJECT_COLUMN:
                {
                    STL_DASSERT( aPrivName != NULL );
                    STL_DASSERT( aNonSchemaName != NULL );
                    STL_DASSERT( aObjectName != NULL );
                    STL_DASSERT( aColumnName != NULL );
                    
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s"
                                 "\nGRANT "
                                 "\n    %s ( \"%s\" ) ON TABLE \"%s\".\"%s\" "
                                 "\n    TO \"%s\" ",
                                 aStringDDL,
                                 aPrivName,
                                 aColumnName,
                                 aNonSchemaName,
                                 aObjectName,
                                 sItem->mGranteeName );
                    break;
                }
            case ZTQ_PRIV_OBJECT_SEQUENCE:
                {
                    STL_DASSERT( aPrivName == NULL );
                    STL_DASSERT( aNonSchemaName != NULL );
                    STL_DASSERT( aObjectName != NULL );
                    STL_DASSERT( aColumnName == NULL );

                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s"
                                 "\nGRANT "
                                 "\n    USAGE ON SEQUENCE \"%s\".\"%s\" "
                                 "\n    TO \"%s\" ",
                                 aStringDDL,
                                 aNonSchemaName,
                                 aObjectName,
                                 sItem->mGranteeName );
                    break;
                }
            default:
                {
                    STL_DASSERT(0);
                    break;
                }
        }

        if ( sItem->mIsGrantable == STL_TRUE )
        {
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n    WITH GRANT OPTION ",
                         aStringDDL );
        }
        else
        {
            /* no grant option */
        }
        
        STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/****************************************************************************
 * DATABASE
 ****************************************************************************/


stlStatus ztqPrintAllDDL( stlChar       * aStringDDL,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack )
{
    /************************************************************************
     * Database DDL
     ************************************************************************/
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Database DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * COMMENT ON DATABASE
     */
    
    STL_TRY( ztqPrintCommentDB( aStringDDL, aAllocator, aErrorStack ) == STL_SUCCESS );
    
    /************************************************************************
     * Tablespace DDL
     ************************************************************************/
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Tablespace DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * CREATE  TABLESPACE
     * ALTER   TABLESPACE
     * COMMENT ON TABLESPACE
     */

    STL_TRY( ztqPrintAllSpaceDDL( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );

    /************************************************************************
     * Profile DDL
     ************************************************************************/
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Profile DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * CREATE PROFILE 
     * COMMENT ON PROFILE
     */

    STL_TRY( ztqPrintAllProfileDDL( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );
    
    
    /************************************************************************
     * Authorization DDL
     ************************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Authorization DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * CREATE  USER
     * COMMENT ON USER
     */

    STL_TRY( ztqPrintAllUserDDL( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );

    /************************************************************************
     * Schema DDL
     ************************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Schema DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * CREATE  SCHEMA
     * COMMENT ON SCHEMA
     */

    STL_TRY( ztqPrintAllSchemaDDL( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );

    /************************************************************************
     * Authorization Schema Path DDL
     ************************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Authorization Schema Path DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * ALTER USER .. SCHEMA PATH
     */
    
    STL_TRY( ztqPrintAllUserSchemaPathDDL( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );
    
    /************************************************************************
     * Database Privilege DDL
     ************************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Database Privilege DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * GRANT .. ON DATABASE
     */
    
    STL_TRY( ztqPrintGrantDB( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );
    
    /************************************************************************
     * Tablespace Privilege DDL
     ************************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Tablespace Privilege DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * GRANT .. ON TABLESPACE
     */
    
    STL_TRY( ztqPrintAllSpaceGRANT( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );
    
    /************************************************************************
     * Schema Privilege DDL
     ************************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Schema Privilege DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * GRANT .. ON SCHEMA
     */
    
    STL_TRY( ztqPrintAllSchemaGRANT( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );

    /************************************************************************
     * Public Synonym DDL
     ************************************************************************/
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Public Synonym DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * CREATE PUBLIC SYNONYM
     */
    
    STL_TRY( ztqPrintAllPublicSynonymDDL( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );
    
    /************************************************************************
     * Table DDL
     ************************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Table DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * CREATE  TABLE
     * COMMENT ON TABLE
     */

    STL_TRY( ztqPrintAllTableDDL( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );

    /************************************************************************
     * Table Privilege DDL
     ************************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Table Privilege DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );
    
    /**
     * GRANT .. ON TABLE
     */
    
    STL_TRY( ztqPrintAllTableGRANT( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );
    
    /************************************************************************
     * appropriate point for uploading data
     ************************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# appropriate point for uploading data "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /************************************************************************
     * Table Option DDL
     ************************************************************************/
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Table Option DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * ALTER TABLE .. ALTER COLUMN .. RESTART
     * ALTER TABLE .. ADD SUPPLEMENTAL LOG
     */

    STL_TRY( ztqPrintAllTableOptionDDL( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );
    
    /************************************************************************
     * Constraint DDL
     ************************************************************************/
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Constraint DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * ALTER TABLE .. ADD CONSTRAINT
     * ALTER TABLE .. ALTER COLUMN .. SET NOT NULL
     * COMMENT ON CONSTRAINT
     */
    
    STL_TRY( ztqPrintAllConstraintDDL( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );
    
    /************************************************************************
     * Index DDL
     ************************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Index DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * CREATE INDEX
     * COMMENT ON INDEX
     */
    
    STL_TRY( ztqPrintAllIndexDDL( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );
    
    /************************************************************************
     * View DDL
     ************************************************************************/
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# View DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * CREATE VIEW
     * ALTER VIEW
     * COMMENT ON TABLE
     */
    
    STL_TRY( ztqPrintAllViewDDL( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );
    
    /************************************************************************
     * View Privilege DDL
     ************************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# View Privilege DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );
    
    /**
     * GRANT .. ON TABLE
     */
    
    STL_TRY( ztqPrintAllViewGRANT( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );
    
    /************************************************************************
     * Sequence DDL
     ************************************************************************/
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Sequence DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * CREATE SEQUENCE
     * ALTER SEQUENCE
     */
    
    STL_TRY( ztqPrintAllSequenceDDL( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );
    
    /************************************************************************
     * Sequence Privilege DDL
     ************************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Sequence Privilege DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );
    
    /**
     * GRANT .. ON SEQUENCE
     */
    
    STL_TRY( ztqPrintAllSequenceGRANT( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );


    /************************************************************************
     * Synonym DDL
     ************************************************************************/
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\n--##################################################### "
                 "\n--# Synonym DDL "
                 "\n--##################################################### " );
    
    STL_TRY( ztqFinishPrintComment( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * CREATE SYNONYM
     */
    
    STL_TRY( ztqPrintAllSynonymDDL( aStringDDL, aAllocator,  aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqPrintAllSpaceDDL( stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSpaceID;
    SQLLEN      sSpaceInd;

    stlChar     sIsBuiltIn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsBuiltInInd;
    
    /************************************************************
     * TABLESPACE 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       TABLESPACE_ID "
                 "     , CAST ( CASE WHEN IS_BUILTIN = TRUE THEN 'YES' "
                 "                                          ELSE 'NO' "
                 "                   END AS VARCHAR(3 OCTETS) ) "
                 "  FROM "
                 "       DEFINITION_SCHEMA.TABLESPACES "
                 " ORDER BY "
                 "       TABLESPACE_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSpaceID,
                            STL_SIZEOF(sSpaceID),
                            & sSpaceInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sIsBuiltIn,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsBuiltInInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * CREATE TABLESPACE 구문 
         */

        if ( stlStrcmp( sIsBuiltIn, "YES" ) == 0  )
        {
            /* nothing to do */
        }
        else
        {
            STL_TRY( ztqPrintCreateSpace( sSpaceID,
                                          aStringDDL,
                                          aAllocator,
                                          aErrorStack )
                     == STL_SUCCESS );
        }

        /**
         * ALTER TABLESPACE 구문 
         */

        STL_TRY( ztqPrintAlterSpace( sSpaceID,
                                     aStringDDL,
                                     aAllocator,
                                     aErrorStack )
                 == STL_SUCCESS );

        /**
         * COMMENT ON TABLESPACE 구문 
         */
        
        if ( stlStrcmp( sIsBuiltIn, "YES" ) == 0  )
        {
            /* nothing to do */
        }
        else
        {
            STL_TRY( ztqPrintCommentSpace( sSpaceID,
                                           aStringDDL,
                                           aAllocator,
                                           aErrorStack )
                     == STL_SUCCESS );
        }
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintAllSpaceGRANT( stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSpaceID;
    SQLLEN      sSpaceInd;
    
    /************************************************************
     * TABLESPACE 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       TABLESPACE_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.TABLESPACES "
                 " ORDER BY "
                 "       TABLESPACE_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSpaceID,
                            STL_SIZEOF(sSpaceID),
                            & sSpaceInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * GRANT .. ON TABLESPACE 구문 
         */

        STL_TRY( ztqPrintGrantSpace( sSpaceID,
                                     aStringDDL,
                                     aAllocator,
                                     aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintAllProfileDDL( stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sProfileID;
    SQLLEN      sProfileInd;

    stlChar     sIsBuiltIn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsBuiltInInd;
    
    /************************************************************
     * PROFILE 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       PROFILE_ID "
                 "     , CAST ( CASE WHEN IS_BUILTIN = TRUE THEN 'YES' "
                 "                                          ELSE 'NO' "
                 "                   END AS VARCHAR(3 OCTETS) ) "
                 "  FROM "
                 "       DEFINITION_SCHEMA.PROFILES "
                 " ORDER BY "
                 "       PROFILE_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sProfileID,
                            STL_SIZEOF(sProfileID),
                            & sProfileInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sIsBuiltIn,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsBuiltInInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * CREATE / ALTER PROFILE 구문 
         */

        STL_TRY( ztqPrintCreateProfile( sProfileID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );

        /**
         * COMMENT ON PROFILE 구문 
         */
        
        if ( stlStrcmp( sIsBuiltIn, "YES" ) == 0  )
        {
            /* nothing to do */
        }
        else
        {
            STL_TRY( ztqPrintCommentProfile( sProfileID,
                                             aStringDDL,
                                             aAllocator,
                                             aErrorStack )
                     == STL_SUCCESS );
        }
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintAllUserDDL( stlChar       * aStringDDL,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sUserID;
    SQLLEN      sUserInd;
    
    /************************************************************
     * USER 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       AUTH_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.AUTHORIZATIONS "
                 " WHERE "
                 "       IS_BUILTIN = FALSE "
                 " ORDER BY "
                 "       AUTH_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sUserID,
                            STL_SIZEOF(sUserID),
                            & sUserInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * CREATE USER 구문 
         */

        STL_TRY( ztqPrintCreateUser( sUserID,
                                     aStringDDL,
                                     aAllocator,
                                     aErrorStack )
                 == STL_SUCCESS );
        
        /**
         * COMMENT ON AUTHORIZATION 구문 
         */
        
        STL_TRY( ztqPrintCommentAuth( sUserID,
                                      aStringDDL,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintAllUserSchemaPathDDL( stlChar       * aStringDDL,
                                        stlAllocator  * aAllocator,
                                        stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sUserID;
    SQLLEN      sUserInd;
    
    /************************************************************
     * USER 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       AUTH_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.AUTHORIZATIONS "
                 " WHERE "
                 "       AUTHORIZATION_TYPE IN ( 'USER', 'PUBLIC' ) "
                 " ORDER BY "
                 "       AUTH_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sUserID,
                            STL_SIZEOF(sUserID),
                            & sUserInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * ALTER USER 구문 
         */

        STL_TRY( ztqPrintAlterUserSchemaPath( sUserID,
                                              aStringDDL,
                                              aAllocator,
                                              aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintAllSchemaDDL( stlChar       * aStringDDL,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSchemaID;
    SQLLEN      sSchemaInd;
    
    /************************************************************
     * SCHEMA 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       SCHEMA_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.SCHEMATA "
                 " WHERE "
                 "       IS_BUILTIN = FALSE "
                 " ORDER BY "
                 "       SCHEMA_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSchemaID,
                            STL_SIZEOF(sSchemaID),
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * CREATE SCHEMA 구문 
         */

        STL_TRY( ztqPrintCreateSchema( sSchemaID,
                                       aStringDDL,
                                       aAllocator,
                                       aErrorStack )
                 == STL_SUCCESS );
        
        /**
         * COMMENT ON SCHEMA 구문 
         */
        
        STL_TRY( ztqPrintCommentSchema( sSchemaID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}




stlStatus ztqPrintAllSchemaGRANT( stlChar       * aStringDDL,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSchemaID;
    SQLLEN      sSchemaInd;
    
    /************************************************************
     * SCHEMA 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       SCHEMA_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.SCHEMATA "
                 " WHERE "
                 "       SCHEMA_ID >= ( SELECT SCHEMA_ID "
                 "                        FROM DEFINITION_SCHEMA.SCHEMATA "
                 "                       WHERE SCHEMA_NAME = 'PUBLIC' "
                 "                    ) "
                 " ORDER BY "
                 "       SCHEMA_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSchemaID,
                            STL_SIZEOF(sSchemaID),
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * GRANT .. ON SCHEMA 구문 
         */

        STL_TRY( ztqPrintGrantSchema( sSchemaID,
                                      aStringDDL,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus ztqPrintAllPublicSynonymDDL( stlChar       * aStringDDL,
                                       stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSynonymID;
    SQLLEN      sSynonymInd;
    
    /************************************************************
     * PUBLIC SYNONYM 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       SYNONYM_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.PUBLIC_SYNONYMS "
                 " WHERE "
                 "       IS_BUILTIN = FALSE "
                 " ORDER BY "
                 "       SYNONYM_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSynonymID,
                            STL_SIZEOF(sSynonymID),
                            & sSynonymInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * CREATE PUBLIC SYNONYM 구문 
         */

        STL_TRY( ztqPrintCreatePublicSynonym( sSynonymID,
                                              aStringDDL,
                                              aAllocator,
                                              aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus ztqPrintAllTableDDL( stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sTableID;
    SQLLEN      sTableInd;
    
    /************************************************************
     * TABLE 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       TABLE_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.TABLES "
                 " WHERE "
                 "       TABLE_TYPE = 'BASE TABLE' "
                 "   AND IS_BUILTIN = FALSE "
                 "   AND SCHEMA_ID >= ( SELECT SCHEMA_ID "
                 "                        FROM DEFINITION_SCHEMA.SCHEMATA "
                 "                       WHERE SCHEMA_NAME = 'PUBLIC' "
                 "                    ) "
                 " ORDER BY "
                 "       SCHEMA_ID "
                 "     , TABLE_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sTableID,
                            STL_SIZEOF(sTableID),
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * CREATE TABLE 구문 
         */

        STL_TRY( ztqPrintCreateTable( sTableID,
                                      aStringDDL,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );
        
        /**
         * COMMENT ON TABLE 구문 
         */
        
        STL_TRY( ztqPrintCommentRelation( sTableID,
                                          aStringDDL,
                                          aAllocator,
                                          aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus ztqPrintAllTableGRANT( stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sTableID;
    SQLLEN      sTableInd;
    
    /************************************************************
     * TABLE 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       TABLE_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.TABLES "
                 " WHERE "
                 "       TABLE_TYPE = 'BASE TABLE' "
                 "   AND IS_BUILTIN = FALSE "
                 "   AND SCHEMA_ID >= ( SELECT SCHEMA_ID "
                 "                        FROM DEFINITION_SCHEMA.SCHEMATA "
                 "                       WHERE SCHEMA_NAME = 'PUBLIC' "
                 "                    ) "
                 " ORDER BY "
                 "       SCHEMA_ID "
                 "     , TABLE_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sTableID,
                            STL_SIZEOF(sTableID),
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * GRANT ON TABLE 구문 
         */
        
        STL_TRY( ztqPrintGrantRelation( sTableID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintAllTableOptionDDL( stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sTableID;
    SQLLEN      sTableInd;
    
    /************************************************************
     * TABLE 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       TABLE_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.TABLES "
                 " WHERE "
                 "       TABLE_TYPE = 'BASE TABLE' "
                 "   AND IS_BUILTIN = FALSE "
                 "   AND SCHEMA_ID >= ( SELECT SCHEMA_ID "
                 "                        FROM DEFINITION_SCHEMA.SCHEMATA "
                 "                       WHERE SCHEMA_NAME = 'PUBLIC' "
                 "                    ) "
                 " ORDER BY "
                 "       SCHEMA_ID "
                 "     , TABLE_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sTableID,
                            STL_SIZEOF(sTableID),
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * ALTER TABLE .. ALTER COLUMN .. RESTART 구문 
         */

        STL_TRY( ztqPrintTableIdentity( sTableID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
        
        /**
         * ALTER TABLE .. SET SUPPLEMENTAL 구문 
         */
        
        STL_TRY( ztqPrintTableSupplemental( sTableID,
                                            aStringDDL,
                                            aAllocator,
                                            aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus ztqPrintAllConstraintDDL( stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sConstID;
    SQLLEN      sConstInd;
    
    /************************************************************
     * CONSTRAINT 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       CONSTRAINT_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.TABLE_CONSTRAINTS "
                 " WHERE "
                 "       IS_BUILTIN = FALSE "
                 " ORDER BY "
                 "       TABLE_SCHEMA_ID "
                 "     , TABLE_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sConstID,
                            STL_SIZEOF(sConstID),
                            & sConstInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * ALTER TABLE 구문 
         */

        STL_TRY( ztqPrintAddConstraint( sConstID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
        
        /**
         * COMMENT ON CONSTRAINT 구문 
         */
        
        STL_TRY( ztqPrintCommentConstraint( sConstID,
                                            aStringDDL,
                                            aAllocator,
                                            aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintAllIndexDDL( stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sIndexID;
    SQLLEN      sIndexInd;
    
    /************************************************************
     * INDEX 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       idx.INDEX_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.INDEXES AS idx "
                 "     , DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE AS ktu "
                 " WHERE "
                 "       idx.BY_CONSTRAINT = FALSE "
                 "   AND idx.IS_BUILTIN = FALSE "
                 "   AND idx.INDEX_ID = ktu.INDEX_ID "
                 " ORDER BY "
                 "       ktu.TABLE_SCHEMA_ID "
                 "     , ktu.TABLE_ID "
                 "     , idx.INDEX_ID " );


    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sIndexID,
                            STL_SIZEOF(sIndexID),
                            & sIndexInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * CREATE INDEX 구문 
         */

        STL_TRY( ztqPrintCreateIndex( sIndexID,
                                      aStringDDL,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );
        
        /**
         * COMMENT ON INDEX 구문 
         */
        
        STL_TRY( ztqPrintCommentIndex( sIndexID,
                                       aStringDDL,
                                       aAllocator,
                                       aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintAllViewDDL( stlChar       * aStringDDL,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sViewID;
    SQLLEN      sViewInd;
    
    /************************************************************
     * CREATE VIEW
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       TABLE_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.TABLES "
                 " WHERE "
                 "       TABLE_TYPE = 'VIEW' "
                 "   AND IS_BUILTIN = FALSE "
                 "   AND SCHEMA_ID >= ( SELECT SCHEMA_ID "
                 "                        FROM DEFINITION_SCHEMA.SCHEMATA "
                 "                       WHERE SCHEMA_NAME = 'PUBLIC' "
                 "                    ) "
                 " ORDER BY "
                 "       TABLE_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sViewID,
                            STL_SIZEOF(sViewID),
                            & sViewInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * CREATE VIEW 구문 
         */

        STL_TRY( ztqPrintCreateView( sViewID,
                                     aStringDDL,
                                     aAllocator,
                                     aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * ALTER VIEW
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       TABLE_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.TABLES "
                 " WHERE "
                 "       TABLE_TYPE = 'VIEW' "
                 "   AND IS_BUILTIN = FALSE "
                 "   AND SCHEMA_ID >= ( SELECT SCHEMA_ID "
                 "                        FROM DEFINITION_SCHEMA.SCHEMATA "
                 "                       WHERE SCHEMA_NAME = 'PUBLIC' "
                 "                    ) "
                 " ORDER BY "
                 "       TABLE_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sViewID,
                            STL_SIZEOF(sViewID),
                            & sViewInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * ALTER VIEW 구문 
         */

        STL_TRY( ztqPrintViewRecompile( sViewID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * COMMENT VIEW
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       TABLE_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.TABLES "
                 " WHERE "
                 "       TABLE_TYPE = 'VIEW' "
                 "   AND IS_BUILTIN = FALSE "
                 "   AND SCHEMA_ID >= ( SELECT SCHEMA_ID "
                 "                        FROM DEFINITION_SCHEMA.SCHEMATA "
                 "                       WHERE SCHEMA_NAME = 'PUBLIC' "
                 "                    ) "
                 " ORDER BY "
                 "       TABLE_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sViewID,
                            STL_SIZEOF(sViewID),
                            & sViewInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * COMMENT ON TABLE 구문 
         */

        STL_TRY( ztqPrintCommentRelation( sViewID,
                                          aStringDDL,
                                          aAllocator,
                                          aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintAllViewGRANT( stlChar       * aStringDDL,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sViewID;
    SQLLEN      sViewInd;
    
    /************************************************************
     * VIEW 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       TABLE_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.TABLES "
                 " WHERE "
                 "       TABLE_TYPE = 'VIEW' "
                 "   AND IS_BUILTIN = FALSE "
                 "   AND SCHEMA_ID >= ( SELECT SCHEMA_ID "
                 "                        FROM DEFINITION_SCHEMA.SCHEMATA "
                 "                       WHERE SCHEMA_NAME = 'PUBLIC' "
                 "                    ) "
                 " ORDER BY "
                 "       SCHEMA_ID "
                 "     , TABLE_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sViewID,
                            STL_SIZEOF(sViewID),
                            & sViewInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * GRANT ON TABLE 구문 
         */
        
        STL_TRY( ztqPrintGrantRelation( sViewID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintAllSequenceDDL( stlChar       * aStringDDL,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSeqID;
    SQLLEN      sSeqInd;
    
    /************************************************************
     * SEQUENCE 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       SEQUENCE_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.SEQUENCES "
                 " WHERE "
                 "       IS_BUILTIN = FALSE "
                 "   AND SCHEMA_ID >= ( SELECT SCHEMA_ID "
                 "                        FROM DEFINITION_SCHEMA.SCHEMATA "
                 "                       WHERE SCHEMA_NAME = 'PUBLIC' "
                 "                    ) "
                 " ORDER BY "
                 "       SCHEMA_ID "
                 "     , SEQUENCE_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSeqID,
                            STL_SIZEOF(sSeqID),
                            & sSeqInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * CREATE Sequence 구문 
         */

        STL_TRY( ztqPrintCreateSequence( sSeqID,
                                         aStringDDL,
                                         aAllocator,
                                         aErrorStack )
                 == STL_SUCCESS );
        
        /**
         * COMMENT ON SEQUENCE 구문 
         */
        
        STL_TRY( ztqPrintCommentSequence( sSeqID,
                                          aStringDDL,
                                          aAllocator,
                                          aErrorStack )
                 == STL_SUCCESS );

        /**
         * ALTER SEQUENCE 구문 
         */
        
        STL_TRY( ztqPrintRestartSequence( sSeqID,
                                          aStringDDL,
                                          aAllocator,
                                          aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintAllSequenceGRANT( stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSeqID;
    SQLLEN      sSeqInd;
    
    /************************************************************
     * SEQUENCE 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       SEQUENCE_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.SEQUENCES "
                 " WHERE "
                 "       IS_BUILTIN = FALSE "
                 "   AND SCHEMA_ID >= ( SELECT SCHEMA_ID "
                 "                        FROM DEFINITION_SCHEMA.SCHEMATA "
                 "                       WHERE SCHEMA_NAME = 'PUBLIC' "
                 "                    ) "
                 " ORDER BY "
                 "       SCHEMA_ID "
                 "     , SEQUENCE_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSeqID,
                            STL_SIZEOF(sSeqID),
                            & sSeqInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * GRANT ON SEQUENCE 구문 
         */

        STL_TRY( ztqPrintGrantSequence( sSeqID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintAllSynonymDDL( stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSynonymID;
    SQLLEN      sSynonymInd;
    
    /************************************************************
     * SYNONYM 목록 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       SYNONYM_ID "
                 "  FROM "
                 "       DEFINITION_SCHEMA.SYNONYMS "
                 " WHERE "
                 "       IS_BUILTIN = FALSE "
                 "   AND SCHEMA_ID >= ( SELECT SCHEMA_ID "
                 "                        FROM DEFINITION_SCHEMA.SCHEMATA "
                 "                       WHERE SCHEMA_NAME = 'PUBLIC' "
                 "                    ) "
                 " ORDER BY "
                 "       SCHEMA_ID "
                 "     , SYNONYM_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSynonymID,
                            STL_SIZEOF(sSynonymID),
                            & sSynonymInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * CREATE SYNONYM 구문 
         */

        STL_TRY( ztqPrintCreateSynonym( sSynonymID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/


    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus ztqPrintGrantDB( stlChar       * aStringDDL,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlRegionMark   sMark;
    stlBool         sIsMarked = STL_FALSE;

    ztqPrivOrder  sPrivOrder;
    
    stlInt64    sOrgPrivType;
    stlChar     sOrgPrivName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    stlInt64    sOwnerID;
    SQLLEN      sOwnerInd;
    
    stlInt64    sGrantorID;
    SQLLEN      sGrantorInd;
    stlChar     sGrantorName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGrantorNameInd;
    
    stlInt64    sGranteeID;
    SQLLEN      sGranteeInd;
    stlChar     sGranteeName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGranteeNameInd;
    stlChar     sIsBuiltIn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsBuiltInInd;
    
    stlInt64    sPrivType;
    SQLLEN      sPrivTypeInd;
    stlChar     sPrivName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sPrivNameInd;
    
    stlChar     sGrantable[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGrantInd;
    
    /**
     * Memory Mark 설정
     */
    
    STL_TRY( stlMarkRegionMem( aAllocator,
                               &sMark,
                               aErrorStack ) == STL_SUCCESS );
    sIsMarked = STL_TRUE;

    /************************************************************
     * Privilege 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       ( SELECT AUTH_ID "
                 "           FROM DEFINITION_SCHEMA.AUTHORIZATIONS "
                 "          WHERE AUTHORIZATION_NAME = '_SYSTEM' ) "
                 "     , grantor.AUTH_ID "
                 "     , grantor.AUTHORIZATION_NAME "
                 "     , grantee.AUTH_ID "
                 "     , grantee.AUTHORIZATION_NAME "
                 "     , CAST( CASE WHEN priv.IS_BUILTIN = TRUE "
                 "                  THEN 'YES' "
                 "                  ELSE 'NO' "
                 "                  END AS VARCHAR(3 OCTETS) ) "
                 "     , priv.PRIVILEGE_TYPE_ID "
                 "     , priv.PRIVILEGE_TYPE "
                 "     , CAST( CASE WHEN priv.IS_GRANTABLE = TRUE THEN 'YES' "
                 "                                                ELSE 'NO' "
                 "             END AS VARCHAR(3 OCTETS) ) "
                 "  FROM "
                 "       DEFINITION_SCHEMA.DATABASE_PRIVILEGES AS priv "
                 "     , DEFINITION_SCHEMA.AUTHORIZATIONS      AS grantor "
                 "     , DEFINITION_SCHEMA.AUTHORIZATIONS      AS grantee "
                 " WHERE "
                 "       priv.GRANTOR_ID  = grantor.AUTH_ID "
                 "   AND priv.GRANTEE_ID  = grantee.AUTH_ID "
                 " ORDER BY "
                 "       priv.PRIVILEGE_TYPE_ID "
                 "     , grantor.AUTH_ID "
                 "     , grantee.AUTH_ID " );
    

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sOwnerID,
                            STL_SIZEOF(sOwnerID),
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_SBIGINT,
                            & sGrantorID,
                            STL_SIZEOF(sGrantorID),
                            & sGrantorInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sGrantorName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGrantorNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_SBIGINT,
                            & sGranteeID,
                            STL_SIZEOF(sGranteeID),
                            & sGranteeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_CHAR,
                            sGranteeName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGranteeNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            6,
                            SQL_C_CHAR,
                            sIsBuiltIn,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsBuiltInInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            7,
                            SQL_C_SBIGINT,
                            & sPrivType,
                            STL_SIZEOF(sPrivType),
                            & sPrivTypeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            8,
                            SQL_C_CHAR,
                            sPrivName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sPrivNameInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            9,
                            SQL_C_CHAR,
                            sGrantable,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGrantInd,
                            aErrorStack )
             == STL_SUCCESS );

    /**
     * 최초 Fetch
     */
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_FINISH );

    /**
     * Privilege Print Order 결정을 위한 초기화
     */
    
    sOrgPrivType = sPrivType;
    stlStrncpy( sOrgPrivName, sPrivName, STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    ztqInitPrivOrder( & sPrivOrder );

    STL_TRY( ztqAddPrivItem( & sPrivOrder,
                             sGrantorID,
                             sGrantorName,
                             sGranteeID,
                             sGranteeName,
                             sIsBuiltIn,
                             sGrantable,
                             aAllocator,
                             aErrorStack )
             == STL_SUCCESS );
    
    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            ztqBuildPrivOrder( & sPrivOrder, sOwnerID );
            
            STL_TRY( ztqPrintGrantByPrivOrder( aStringDDL,
                                               & sPrivOrder,
                                               ZTQ_PRIV_OBJECT_DATABASE,
                                               sOrgPrivName,
                                               NULL, /* aNonSchemaName */
                                               NULL, /* aObjectName */
                                               NULL, /* aColumnName */
                                               aErrorStack )
                     == STL_SUCCESS );
            break;
        }

        if ( sPrivType == sOrgPrivType )
        {
            /**
             * 이전과 동일한 Privilege Type 인 경우
             */

            STL_TRY( ztqAddPrivItem( & sPrivOrder,
                                     sGrantorID,
                                     sGrantorName,
                                     sGranteeID,
                                     sGranteeName,
                                     sIsBuiltIn,
                                     sGrantable,
                                     aAllocator,
                                     aErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * 서로 다른 Privilege Type 인 경우
             */

            ztqBuildPrivOrder( & sPrivOrder, sOwnerID );
            
            STL_TRY( ztqPrintGrantByPrivOrder( aStringDDL,
                                               & sPrivOrder,
                                               ZTQ_PRIV_OBJECT_DATABASE,
                                               sOrgPrivName,
                                               NULL, /* aNonSchemaName */
                                               NULL, /* aObjectName */
                                               NULL, /* aColumnName */
                                               aErrorStack )
                     == STL_SUCCESS );

            /**
             * 메모리 재사용
             */
            
            sIsMarked = STL_FALSE;
            STL_TRY( stlRestoreRegionMem( aAllocator,
                                          &sMark,
                                          STL_FALSE, /* aFree */
                                          aErrorStack ) == STL_SUCCESS );
            STL_TRY( stlMarkRegionMem( aAllocator,
                                       &sMark,
                                       aErrorStack ) == STL_SUCCESS );
            sIsMarked = STL_TRUE;

            /**
             * Privilege Print Order 결정을 위한 초기화
             */
            
            sOrgPrivType = sPrivType;
            stlStrncpy( sOrgPrivName, sPrivName, STL_MAX_SQL_IDENTIFIER_LENGTH );
            
            ztqInitPrivOrder( & sPrivOrder );
            
            STL_TRY( ztqAddPrivItem( & sPrivOrder,
                                     sGrantorID,
                                     sGrantorName,
                                     sGranteeID,
                                     sGranteeName,
                                     sIsBuiltIn,
                                     sGrantable,
                                     aAllocator,
                                     aErrorStack )
                     == STL_SUCCESS );
        }
    }
    
    /************************************************************
     * 마무리 
     ************************************************************/

    STL_RAMP( RAMP_FINISH );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /**
     * Memory Mark 해제
     */
    
    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        STL_TRY( stlRestoreRegionMem( aAllocator,
                                      &sMark,
                                      STL_FALSE, /* aFree */
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        (void) stlRestoreRegionMem( aAllocator,
                                    &sMark,
                                    STL_FALSE, /* aFree */
                                    aErrorStack );
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintCommentDB( stlChar       * aStringDDL,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlChar     sComment[ZTQ_MAX_COMMENT_LENGTH];
    SQLLEN      sCommentInd;

    /************************************************************
     * COMMENT 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        COMMENTS             "
                 "   FROM "
                 "        DEFINITION_SCHEMA.CATALOG_NAME  " );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sComment,
                            ZTQ_MAX_COMMENT_LENGTH,
                            & sCommentInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * COMMENT ON TABLESPACE 구문 생성
     ************************************************************/

    if ( sCommentInd == SQL_NULL_DATA )
    {
        /* comment 가 없는 경우 */
    }
    else
    {
        /**
         * SET SESSION AUTHORIZATION owner
         */
    
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "\nSET SESSION AUTHORIZATION \"SYS\"; " );
    
        /**
         * COMMENT
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\nCOMMENT "
                     "\n    ON DATABASE ",
                     aStringDDL );

        /**
         * IS 'comment'
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    IS '%s' ",
                     aStringDDL,
                     sComment );
    
        STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



/****************************************************************************
 * Tablespace
 ****************************************************************************/



stlStatus ztqGetSpaceID( ztqIdentifier * aIdentifier,
                         stlAllocator  * aAllocator,
                         stlInt64      * aSpaceID,
                         stlBool       * aIsBuiltIn,
                         stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt32    sLen = 0;
    stlChar     sSpaceName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    stlInt64    sSpaceID;
    SQLLEN      sSpaceInd;
    stlChar     sIsBuiltIn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsBuiltInInd;

    /************************************************************
     * User Name 획득
     ************************************************************/
    
    if( aIdentifier->mIdentifier[0] == '"' )
    {
        sLen = stlStrlen( aIdentifier->mIdentifier );
        stlMemcpy( sSpaceName, aIdentifier->mIdentifier + 1, sLen - 2 );
        sSpaceName[sLen-2] = '\0';
    }
    else
    {
        stlStrcpy( sSpaceName, aIdentifier->mIdentifier );
    }

    /************************************************************
     * TABLESPACE 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT TABLESPACE_ID "
                 "     , CAST( CASE WHEN IS_BUILTIN = TRUE "
                 "                  THEN 'YES' "
                 "                  ELSE 'NO' "
                 "                  END AS VARCHAR(3 OCTETS) ) "
                 "  FROM DEFINITION_SCHEMA.TABLESPACES "
                 " WHERE TABLESPACE_NAME = '%s' ",
                 sSpaceName );

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSpaceID,
                            STL_SIZEOF(sSpaceID),
                            & sSpaceInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sIsBuiltIn,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsBuiltInInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aSpaceID = sSpaceID;

    if ( stlStrcmp( sIsBuiltIn, "YES" ) == 0 )
    {
        *aIsBuiltIn = STL_TRUE;
    }
    else
    {
        *aIsBuiltIn = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintCreateSpace( stlInt64        aSpaceID,
                               stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sSpaceName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSpaceInd;
    stlChar     sMediaType[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sMediaInd;
    stlChar     sUsageType[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sUsageInd;
    stlChar     sDataFile[STL_PATH_MAX];
    SQLLEN      sDataFileInd;
    stlInt64    sFileSize;
    SQLLEN      sFileSizeInd;
    stlChar     sOnOff[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOnOffInd;
    stlChar     sLogging[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sLoggingInd;
    stlInt64    sExtSize;
    SQLLEN      sExtSizeInd;

    stlBool     sIsDataSpace = STL_FALSE;
    
    /************************************************************
     * CREATE TABLESPACE 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT spc.TABLESPACE_NAME "
                 "     , spc.MEDIA_TYPE "
                 "     , spc.USAGE_TYPE "
                 "     , xdf.PATH "
                 "     , xdf.SIZE "
                 "     , CAST( CASE WHEN xspc.ONLINE = TRUE "
                 "                  THEN 'ONLINE' "
                 "                  ELSE 'OFFLINE' "
                 "                  END "
                 "             AS VARCHAR(32 OCTETS) ) "
                 "     , CAST( CASE WHEN xspc.LOGGING = TRUE "
                 "                  THEN 'LOGGING' "
                 "                  ELSE 'NOLOGGING' "
                 "                  END "
                 "             AS VARCHAR(32 OCTETS) ) "
                 "     , xspc.EXTSIZE "
                 "  FROM DEFINITION_SCHEMA.TABLESPACES spc "
                 "     , FIXED_TABLE_SCHEMA.X$TABLESPACE xspc "
                 "     , FIXED_TABLE_SCHEMA.X$DATAFILE xdf "
                 " WHERE spc.TABLESPACE_ID = xspc.ID "
                 "   AND spc.TABLESPACE_ID = xdf.TABLESPACE_ID "
                 "   AND xdf.DATAFILE_ID   = 0 "
                 "   AND spc.TABLESPACE_ID = %ld ",
                 aSpaceID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sSpaceName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSpaceInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sMediaType,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sMediaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sUsageType,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sUsageInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sDataFile,
                            STL_PATH_MAX,
                            & sDataFileInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_SBIGINT,
                            & sFileSize,
                            STL_PATH_MAX,
                            & sFileSizeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            6,
                            SQL_C_CHAR,
                            sOnOff,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOnOffInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            7,
                            SQL_C_CHAR,
                            sLogging,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sLoggingInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            8,
                            SQL_C_SBIGINT,
                            & sExtSize,
                            STL_SIZEOF(sExtSize),
                            & sExtSizeInd,
                            aErrorStack )
             == STL_SUCCESS );
    

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    if ( stlStrcmp( sUsageType, "DATA" ) == 0 )
    {
        sIsDataSpace = STL_TRUE;
    }
    else if ( stlStrcmp( sUsageType, "TEMPORARY" ) == 0 )
    {
        sIsDataSpace = STL_FALSE;
    }
    else
    {
        /**
         * dictionary tablespace 와 undo tablespace 는 CREATE TABLESPACE 구문이 없다.
         */

        STL_THROW( RAMP_ERR_INVALID_OBJECT_IDENTIFIER );
    }
    
    /************************************************************
     * CREATE TABLESPACE 구문 생성
     ************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"SYS\"; " );
    
    /**
     * CREATE TABLESPACE
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nCREATE %s %s TABLESPACE \"%s\" ",
                 aStringDDL,
                 sMediaType,
                 sUsageType,
                 sSpaceName );

    if ( sIsDataSpace == STL_TRUE )
    {
        /**
         * DATA TABLESPACE 인 경우
         */
        
        /**
         * DATAFILE
         */
        
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    DATAFILE \'%s\' "
                     "\n        SIZE %ld REUSE ",
                     aStringDDL,
                     sDataFile,
                     sFileSize );
        
        /**
         * tablespace attribute
         */
        
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    %s "
                     "\n    %s "
                     "\n    EXTSIZE %ld ",
                     aStringDDL,
                     sOnOff,
                     sLogging,
                     sExtSize );
    }
    else
    {
        /**
         * TEMPORARY TABLESPACE 인 경우
         */

        /**
         * MEMORY
         */
        
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    MEMORY \'%s\' "
                     "\n        SIZE %ld ",
                     aStringDDL,
                     sDataFile,
                     sFileSize );
        
        /**
         * tablespace attribute
         */
        
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    EXTSIZE %ld ",
                     aStringDDL,
                     sExtSize );
    }
    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintAlterSpace( stlInt64        aSpaceID,
                              stlChar       * aStringDDL,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sSpaceName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSpaceInd;
    stlChar     sUsageType[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sUsageInd;
    stlChar     sDataFile[STL_PATH_MAX];
    SQLLEN      sDataFileInd;
    stlInt64    sFileSize;
    SQLLEN      sFileSizeInd;

    stlBool     sIsDataFile = STL_FALSE;
    
    /************************************************************
     * ALTER TABLESPACE 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT spc.TABLESPACE_NAME "
                 "     , spc.USAGE_TYPE "
                 "     , xdf.PATH "
                 "     , xdf.SIZE "
                 "  FROM DEFINITION_SCHEMA.TABLESPACES spc "
                 "     , FIXED_TABLE_SCHEMA.X$DATAFILE xdf "
                 " WHERE spc.TABLESPACE_ID = xdf.TABLESPACE_ID "
                 "   AND xdf.DATAFILE_ID   > 0 "
                 "   AND spc.TABLESPACE_ID = %ld "
                 " ORDER BY "
                 "       xdf.DATAFILE_ID ",
                 aSpaceID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sSpaceName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSpaceInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sUsageType,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sUsageInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sDataFile,
                            STL_PATH_MAX,
                            & sDataFileInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_SBIGINT,
                            & sFileSize,
                            STL_PATH_MAX,
                            & sFileSizeInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        if ( stlStrcmp( sUsageType, "TEMPORARY" ) == 0 )
        {
            sIsDataFile = STL_FALSE;
        }
        else
        {
            sIsDataFile = STL_TRUE;
        }

        /************************************************************
         * ALTER TABLESPACE 구문 생성
         ************************************************************/

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "\nSET SESSION AUTHORIZATION \"SYS\"; " );
        
        /**
         * ALTER TABLESPACE
         */
        
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\nALTER TABLESPACE \"%s\" ",
                     aStringDDL,
                     sSpaceName );

        if ( sIsDataFile == STL_TRUE )
        {
            /**
             * ADD DATAFILE 인 경우
             */
            
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n    ADD DATAFILE \'%s\' "
                         "\n        SIZE %ld REUSE ",
                         aStringDDL,
                         sDataFile,
                         sFileSize );
            
        }
        else
        {
            /**
             *  ADD MEMORY 인 경우
             */
        
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n    ADD MEMORY \'%s\' "
                         "\n        SIZE %ld ",
                         aStringDDL,
                         sDataFile,
                         sFileSize );
        }
    
        STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
    }
        

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintSpaceCreateTable( stlInt64        aSpaceID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sTableID;
    SQLLEN      sTableInd;
    
    /************************************************************
     * TABLE 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT TABLE_ID "
                 "  FROM DEFINITION_SCHEMA.TABLES "
                 " WHERE TABLE_TYPE = 'BASE TABLE' "
                 "   AND TABLESPACE_ID = %ld "
                 " ORDER BY SCHEMA_ID, TABLE_ID ",
                 aSpaceID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sTableID,
                            STL_SIZEOF(sTableID),
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * TableID 에 해당하는 CREATE TABLE 구문 출력
         */

        STL_TRY( ztqPrintCreateTable( sTableID,
                                      aStringDDL,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintSpaceConstraint( stlInt64        aSpaceID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sConstID;
    SQLLEN      sConstInd;
    
    /************************************************************
     * CONSTRAINT 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT cst.CONSTRAINT_ID "
                 "  FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS cst "
                 "     , DEFINITION_SCHEMA.INDEXES idx "
                 "     , DEFINITION_SCHEMA.TABLESPACES spc "
                 " WHERE cst.ASSOCIATED_INDEX_ID = idx.INDEX_ID "
                 "   AND idx.TABLESPACE_ID = spc.TABLESPACE_ID "
                 "   AND cst.CONSTRAINT_TYPE IN ( 'NOT NULL', 'PRIMARY KEY', 'UNIQUE' ) "
                 "   AND spc.TABLESPACE_ID = %ld "
                 " ORDER BY "
                 "       cst.TABLE_SCHEMA_ID "
                 "     , cst.TABLE_ID "
                 "     , CASE WHEN cst.CONSTRAINT_TYPE = 'NOT NULL'    THEN 1 "
                 "            WHEN cst.CONSTRAINT_TYPE = 'PRIMARY KEY' THEN 2 "
                 "            WHEN cst.CONSTRAINT_TYPE = 'UNIQUE'      THEN 3 "
                 "            ELSE NULL "
                 "            END "
                 "     , cst.CONSTRAINT_ID ",
                 aSpaceID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sConstID,
                            STL_SIZEOF(sConstID),
                            & sConstInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        STL_TRY( ztqPrintAddConstraint( sConstID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintSpaceCreateIndex( stlInt64        aSpaceID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sIndexID;
    SQLLEN      sIndexInd;
    
    /************************************************************
     * INDEX 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT idx.INDEX_ID "
                 "  FROM DEFINITION_SCHEMA.INDEXES idx"
                 "     , DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE ikey"
                 " WHERE idx.INDEX_ID = ikey.INDEX_ID "
                 "   AND idx.BY_CONSTRAINT = FALSE "
                 "   AND idx.TABLESPACE_ID = %ld "
                 " ORDER BY "
                 "       ikey.TABLE_SCHEMA_ID "
                 "     , ikey.TABLE_ID "
                 "     , ikey.INDEX_ID ",
                 aSpaceID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sIndexID,
                            STL_SIZEOF(sIndexID),
                            & sIndexInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * IndexID 에 해당하는 CREATE INDEX 구문 출력
         */

        STL_TRY( ztqPrintCreateIndex( sIndexID,
                                      aStringDDL,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintGrantSpace( stlInt64        aSpaceID,
                              stlChar       * aStringDDL,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlRegionMark   sMark;
    stlBool         sIsMarked = STL_FALSE;

    ztqPrivOrder  sPrivOrder;
    
    stlInt64    sOrgPrivType;
    stlChar     sOrgPrivName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    stlInt64    sOwnerID;
    SQLLEN      sOwnerInd;

    stlInt64    sGrantorID;
    SQLLEN      sGrantorInd;
    stlChar     sGrantorName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGrantorNameInd;
    
    stlInt64    sGranteeID;
    SQLLEN      sGranteeInd;
    stlChar     sGranteeName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGranteeNameInd;
    stlChar     sIsBuiltIn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsBuiltInInd;

    stlChar     sSpaceName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSpaceInd;
    
    stlInt64    sPrivType;
    SQLLEN      sPrivTypeInd;
    stlChar     sPrivName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sPrivNameInd;
    
    stlChar     sGrantable[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGrantInd;

    /**
     * Memory Mark 설정
     */
    
    STL_TRY( stlMarkRegionMem( aAllocator,
                               &sMark,
                               aErrorStack ) == STL_SUCCESS );
    sIsMarked = STL_TRUE;
    
    /************************************************************
     * GRANT priv ON TABLESPACE 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       ( SELECT AUTH_ID "
                 "           FROM DEFINITION_SCHEMA.AUTHORIZATIONS "
                 "          WHERE AUTHORIZATION_NAME = '_SYSTEM' ) "
                 "     , grantor.AUTH_ID "
                 "     , grantor.AUTHORIZATION_NAME "
                 "     , grantee.AUTH_ID "
                 "     , grantee.AUTHORIZATION_NAME "
                 "     , CAST( CASE WHEN priv.IS_BUILTIN = TRUE "
                 "                       OR "
                 "                       priv.GRANTEE_ID = ( SELECT AUTH_ID "
                 "                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS "
                 "                                            WHERE AUTHORIZATION_NAME = 'SYS' ) "
                 "                  THEN 'YES' "
                 "                  ELSE 'NO' "
                 "                  END AS VARCHAR(3 OCTETS) ) "
                 "     , spc.TABLESPACE_NAME "
                 "     , priv.PRIVILEGE_TYPE_ID "
                 "     , priv.PRIVILEGE_TYPE "
                 "     , CAST( CASE WHEN priv.IS_GRANTABLE = TRUE THEN 'YES' "
                 "                                                ELSE 'NO' "
                 "             END AS VARCHAR(3 OCTETS) ) "
                 "  FROM "
                 "       DEFINITION_SCHEMA.TABLESPACE_PRIVILEGES  AS priv "
                 "     , DEFINITION_SCHEMA.TABLESPACES            AS spc "
                 "     , DEFINITION_SCHEMA.AUTHORIZATIONS         AS grantor "
                 "     , DEFINITION_SCHEMA.AUTHORIZATIONS         AS grantee "
                 " WHERE "
                 "       spc.TABLESPACE_ID = priv.TABLESPACE_ID "
                 "   AND priv.GRANTOR_ID   = grantor.AUTH_ID "
                 "   AND priv.GRANTEE_ID   = grantee.AUTH_ID "
                 "   AND spc.TABLESPACE_ID = %ld "
                 " ORDER BY "
                 "       priv.PRIVILEGE_TYPE_ID "
                 "     , grantor.AUTH_ID "
                 "     , grantee.AUTH_ID ",
                 aSpaceID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sOwnerID,
                            STL_SIZEOF(sOwnerID),
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_SBIGINT,
                            & sGrantorID,
                            STL_SIZEOF(sGrantorID),
                            & sGrantorInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sGrantorName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGrantorNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_SBIGINT,
                            & sGranteeID,
                            STL_SIZEOF(sGranteeID),
                            & sGranteeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_CHAR,
                            sGranteeName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGranteeNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            6,
                            SQL_C_CHAR,
                            sIsBuiltIn,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsBuiltInInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            7,
                            SQL_C_CHAR,
                            sSpaceName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSpaceInd,
                            aErrorStack )
             == STL_SUCCESS );

    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            8,
                            SQL_C_SBIGINT,
                            & sPrivType,
                            STL_SIZEOF(sPrivType),
                            & sPrivTypeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            9,
                            SQL_C_CHAR,
                            sPrivName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sPrivNameInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            10,
                            SQL_C_CHAR,
                            sGrantable,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGrantInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    /**
     * 최초 Fetch
     */
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_FINISH );

    /**
     * Privilege Print Order 결정을 위한 초기화
     */
    
    sOrgPrivType = sPrivType;
    stlStrncpy( sOrgPrivName, sPrivName, STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    ztqInitPrivOrder( & sPrivOrder );

    STL_TRY( ztqAddPrivItem( & sPrivOrder,
                             sGrantorID,
                             sGrantorName,
                             sGranteeID,
                             sGranteeName,
                             sIsBuiltIn,
                             sGrantable,
                             aAllocator,
                             aErrorStack )
             == STL_SUCCESS );
    
    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            ztqBuildPrivOrder( & sPrivOrder, sOwnerID );
            
            STL_TRY( ztqPrintGrantByPrivOrder( aStringDDL,
                                               & sPrivOrder,
                                               ZTQ_PRIV_OBJECT_TABLESPACE,
                                               sOrgPrivName,
                                               sSpaceName, 
                                               NULL, /* aObjectName */
                                               NULL, /* aColumnName */
                                               aErrorStack )
                     == STL_SUCCESS );
            break;
        }

        if ( sPrivType == sOrgPrivType )
        {
            /**
             * 이전과 동일한 Privilege Type 인 경우
             */

            STL_TRY( ztqAddPrivItem( & sPrivOrder,
                                     sGrantorID,
                                     sGrantorName,
                                     sGranteeID,
                                     sGranteeName,
                                     sIsBuiltIn,
                                     sGrantable,
                                     aAllocator,
                                     aErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * 서로 다른 Privilege Type 인 경우
             */

            ztqBuildPrivOrder( & sPrivOrder, sOwnerID );
            
            STL_TRY( ztqPrintGrantByPrivOrder( aStringDDL,
                                               & sPrivOrder,
                                               ZTQ_PRIV_OBJECT_TABLESPACE,
                                               sOrgPrivName,
                                               sSpaceName, 
                                               NULL, /* aObjectName */
                                               NULL, /* aColumnName */
                                               aErrorStack )
                     == STL_SUCCESS );

            /**
             * 메모리 재사용
             */
            
            sIsMarked = STL_FALSE;
            STL_TRY( stlRestoreRegionMem( aAllocator,
                                          &sMark,
                                          STL_FALSE, /* aFree */
                                          aErrorStack ) == STL_SUCCESS );
            STL_TRY( stlMarkRegionMem( aAllocator,
                                       &sMark,
                                       aErrorStack ) == STL_SUCCESS );
            sIsMarked = STL_TRUE;

            /**
             * Privilege Print Order 결정을 위한 초기화
             */
            
            sOrgPrivType = sPrivType;
            stlStrncpy( sOrgPrivName, sPrivName, STL_MAX_SQL_IDENTIFIER_LENGTH );
            
            ztqInitPrivOrder( & sPrivOrder );
            
            STL_TRY( ztqAddPrivItem( & sPrivOrder,
                                     sGrantorID,
                                     sGrantorName,
                                     sGranteeID,
                                     sGranteeName,
                                     sIsBuiltIn,
                                     sGrantable,
                                     aAllocator,
                                     aErrorStack )
                     == STL_SUCCESS );
        }
    }

    /************************************************************
     * 마무리 
     ************************************************************/

    STL_RAMP( RAMP_FINISH );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /**
     * Memory Mark 해제
     */
    
    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        STL_TRY( stlRestoreRegionMem( aAllocator,
                                      &sMark,
                                      STL_FALSE, /* aFree */
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        (void) stlRestoreRegionMem( aAllocator,
                                    &sMark,
                                    STL_FALSE, /* aFree */
                                    aErrorStack );
    }
    
    return STL_FAILURE;
}




stlStatus ztqPrintCommentSpace( stlInt64        aSpaceID,
                                stlChar       * aStringDDL,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlChar     sSpaceName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSpaceInd;
    stlChar     sComment[ZTQ_MAX_COMMENT_LENGTH];
    SQLLEN      sCommentInd;

    /************************************************************
     * COMMENT 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        spc.TABLESPACE_NAME  "
                 "      , spc.COMMENTS             "
                 "   FROM "
                 "        DEFINITION_SCHEMA.TABLESPACES AS spc  "
                 "  WHERE "
                 "        spc.TABLESPACE_ID = %ld ",
                 aSpaceID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sSpaceName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSpaceInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sComment,
                            ZTQ_MAX_COMMENT_LENGTH,
                            & sCommentInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * COMMENT ON TABLESPACE 구문 생성
     ************************************************************/

    if ( sCommentInd == SQL_NULL_DATA )
    {
        /* comment 가 없는 경우 */
    }
    else
    {
        /**
         * SET SESSION AUTHORIZATION owner
         */
    
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "\nSET SESSION AUTHORIZATION \"SYS\"; " );
    
        /**
         * COMMENT
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\nCOMMENT "
                     "\n    ON TABLESPACE \"%s\" ",
                     aStringDDL,
                     sSpaceName );

        /**
         * IS 'comment'
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    IS '%s' ",
                     aStringDDL,
                     sComment );
    
        STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


/****************************************************************************
 * Profile
 ****************************************************************************/



stlStatus ztqGetProfileID( ztqIdentifier * aIdentifier,
                           stlAllocator  * aAllocator,
                           stlInt64      * aProfileID,
                           stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt32    sLen = 0;
    stlChar     sProfileName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    stlInt64    sProfileID;
    SQLLEN      sProfileInd;

    /************************************************************
     * Profile Name 획득
     ************************************************************/
    
    if( aIdentifier->mIdentifier[0] == '"' )
    {
        sLen = stlStrlen( aIdentifier->mIdentifier );
        stlMemcpy( sProfileName, aIdentifier->mIdentifier + 1, sLen - 2 );
        sProfileName[sLen-2] = '\0';
    }
    else
    {
        stlStrcpy( sProfileName, aIdentifier->mIdentifier );
    }

    /************************************************************
     * PROFILE 관련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT PROFILE_ID "
                 "  FROM DEFINITION_SCHEMA.PROFILES "
                 " WHERE PROFILE_NAME = '%s' ",
                 sProfileName );

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sProfileID,
                            STL_SIZEOF(sProfileID),
                            & sProfileInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aProfileID = sProfileID;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintCreateProfile( stlInt64        aProfileID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sProfileName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sProfileInd;
    stlChar     sIsBuiltIn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsBuiltInInd;
    stlChar     sParamName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sParamInd;
    stlChar   * sValueString;
    SQLLEN      sValueInd;

    stlBool   sFirstRow = STL_FALSE;

    stlRegionMark   sMark;
    stlBool         sIsMarked = STL_FALSE;
    
    /**
     * PARAMETER_STRING 공간 확보
     */

    STL_TRY( stlMarkRegionMem( aAllocator,
                               &sMark,
                               aErrorStack ) == STL_SUCCESS );

    sIsMarked = STL_TRUE;
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sValueString,
                                aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * CREATE PROFILE 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT prof.PROFILE_NAME "
                 "     , CAST( CASE WHEN prof.IS_BUILTIN = TRUE THEN 'YES' "
                 "                                              ELSE 'NO' "
                 "                   END AS VARCHAR(3 OCTETS) ) "
                 "     , pram.PARAMETER_NAME "
                 "     , pram.PARAMETER_STRING "
                 "  FROM DEFINITION_SCHEMA.PROFILES prof "
                 "     , DEFINITION_SCHEMA.PROFILE_PASSWORD_PARAMETER pram "
                 " WHERE prof.PROFILE_ID = pram.PROFILE_ID "
                 "   AND prof.PROFILE_ID = %ld "
                 " ORDER BY "
                 "       pram.PARAMETER_ID ",
                 aProfileID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sProfileName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sProfileInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sIsBuiltIn,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsBuiltInInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sParamName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sParamInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sValueString,
                            gZtqDdlSize,
                            & sValueInd,
                            aErrorStack )
             == STL_SUCCESS );

    sFirstRow = STL_TRUE;
    
    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            if ( sFirstRow == STL_TRUE )
            {
                STL_THROW( RAMP_ERR_INVALID_OBJECT_IDENTIFIER );
            }
            break;
        }

        if ( sResult == SQL_SUCCESS_WITH_INFO )
        {
            /**
             * ValueString 값이 Buffer 공간을 초과한 경우 character length of data greater than column length
             * - 그 외의 경우라도 문제를 유발할 수 있으므로, 정확한 ERROR_CODE 검사 없이 에러를 낸다.
             */
            if ( sValueInd >= gZtqDdlSize )
            {
                STL_THROW( RAMP_ERR_NOT_ENOUGH_DDLSIZE );
            }
            else
            {
                /**
                 * ztqSQLFetch() 에서 메시지를 출력함
                 */
                STL_TRY( 0 );
            }
        }
        
        if ( sFirstRow == STL_TRUE )
        {
            sFirstRow = STL_FALSE;
            
            /************************************************************
             * CREATE PROFILE 구문 생성
             ************************************************************/
            
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "\nSET SESSION AUTHORIZATION \"SYS\"; " );
            
            if ( stlStrcmp( sIsBuiltIn, "YES" ) == 0 )
            {
                /**
                 * ALTER PROFILE
                 */
                
                stlSnprintf( aStringDDL,
                             gZtqDdlSize,
                             "%s"
                             "\nALTER PROFILE \"%s\" LIMIT ",
                             aStringDDL,
                             sProfileName );
            }
            else
            {
                /**
                 * CREATE PROFILE
                 */
                
                stlSnprintf( aStringDDL,
                             gZtqDdlSize,
                             "%s"
                             "\nCREATE PROFILE \"%s\" LIMIT ",
                             aStringDDL,
                             sProfileName );
                
            }
        }

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    %s   %s ",
                     aStringDDL,
                     sParamName,
                     sValueString );
    }

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        STL_TRY( stlRestoreRegionMem( aAllocator,
                                      &sMark,
                                      STL_FALSE, /* aFree */
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_NOT_ENOUGH_DDLSIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_ENOUGH_DDLSIZE,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintCommentProfile( stlInt64        aProfileID,
                                  stlChar       * aStringDDL,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlChar     sProfileName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sProfileInd;
    stlChar     sComment[ZTQ_MAX_COMMENT_LENGTH];
    SQLLEN      sCommentInd;

    /************************************************************
     * COMMENT 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        prof.PROFILE_NAME  "
                 "      , prof.COMMENTS             "
                 "   FROM "
                 "        DEFINITION_SCHEMA.PROFILES AS prof  "
                 "  WHERE "
                 "        prof.PROFILE_ID = %ld ",
                 aProfileID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sProfileName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sProfileInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sComment,
                            ZTQ_MAX_COMMENT_LENGTH,
                            & sCommentInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * COMMENT ON PROFILE 구문 생성
     ************************************************************/

    if ( sCommentInd == SQL_NULL_DATA )
    {
        /* comment 가 없는 경우 */
    }
    else
    {
        /**
         * SET SESSION AUTHORIZATION
         */
    
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "\nSET SESSION AUTHORIZATION \"SYS\"; " );
    
        /**
         * COMMENT
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\nCOMMENT "
                     "\n    ON PROFILE \"%s\" ",
                     aStringDDL,
                     sProfileName );

        /**
         * IS 'comment'
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    IS '%s' ",
                     aStringDDL,
                     sComment );
    
        STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


/****************************************************************************
 * Authorization
 ****************************************************************************/


stlStatus ztqGetAuthID( ztqIdentifier * aIdentifier,
                        stlAllocator  * aAllocator,
                        stlInt64      * aAuthID,
                        stlBool       * aIsUser,
                        stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt32    sLen = 0;
    stlChar     sAuthName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    stlInt64    sAuthID;
    SQLLEN      sAuthInd;
    stlChar     sAuthType[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sAuthTypeInd;

    /************************************************************
     * User Name 획득
     ************************************************************/
    
    if( aIdentifier->mIdentifier[0] == '"' )
    {
        sLen = stlStrlen( aIdentifier->mIdentifier );
        stlMemcpy( sAuthName, aIdentifier->mIdentifier + 1, sLen - 2 );
        sAuthName[sLen-2] = '\0';
    }
    else
    {
        stlStrcpy( sAuthName, aIdentifier->mIdentifier );
    }

    /************************************************************
     * USER 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT AUTH_ID "
                 "     , AUTHORIZATION_TYPE "
                 "  FROM DEFINITION_SCHEMA.AUTHORIZATIONS "
                 " WHERE AUTHORIZATION_NAME = '%s' ",
                 sAuthName );

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sAuthID,
                            STL_SIZEOF(sAuthID),
                            & sAuthInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sAuthType,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sAuthTypeInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aAuthID = sAuthID;

    if ( stlStrcmp( sAuthType, "USER" ) == 0 )
    {
        *aIsUser = STL_TRUE;
    }
    else
    {
        *aIsUser = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintCreateUser( stlInt64        aUserID,
                              stlChar       * aStringDDL,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sUserName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sUserInd;
    stlChar     sDataSpace[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sDataInd;
    stlChar     sTempSpace[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sTempInd;
    stlChar     sProfileName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sProfileInd;

    /************************************************************
     * CREATE USER 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT auth.AUTHORIZATION_NAME "
                 "     , ( SELECT spc.TABLESPACE_NAME "
                 "           FROM DEFINITION_SCHEMA.TABLESPACES AS spc "
                 "          WHERE spc.TABLESPACE_ID = usr.DEFAULT_DATA_TABLESPACE_ID ) "
                 "     , ( SELECT spc.TABLESPACE_NAME "
                 "           FROM DEFINITION_SCHEMA.TABLESPACES AS spc "
                 "          WHERE spc.TABLESPACE_ID = usr.DEFAULT_TEMP_TABLESPACE_ID ) "
                 "     , ( SELECT prof.PROFILE_NAME "
                 "           FROM DEFINITION_SCHEMA.PROFILES AS prof "
                 "          WHERE prof.PROFILE_ID = usr.PROFILE_ID ) "
                 "  FROM DEFINITION_SCHEMA.AUTHORIZATIONS auth, "
                 "       DEFINITION_SCHEMA.USERS usr "
                 " WHERE auth.AUTH_ID = %ld "
                 "   AND auth.AUTH_ID = usr.AUTH_ID ",
                 aUserID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sUserName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sUserInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sDataSpace,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sDataInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sTempSpace,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sTempInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sProfileName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sProfileInd,
                            aErrorStack )
             == STL_SUCCESS );
    

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * CREATE USER 구문 생성
     ************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"SYS\"; " );
    
    /**
     * CREATE USER
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nCREATE USER \"%s\" ",
                 aStringDDL,
                 sUserName );
    
    /**
     * IDENTIIFIED BY
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    IDENTIFIED BY %s ",
                 aStringDDL,
                 ZTQ_UNKNOWN_PASSWORD );

    /**
     * DEFAULT TABLESPACE
     */
    
    if ( sDataInd == SQL_NULL_DATA )
    {
        /**
         * User 의 default tablespace 가 DROP 된 경우
         */
    }
    else
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    DEFAULT TABLESPACE \"%s\" ",
                     aStringDDL,
                     sDataSpace );
    }
    
    /**
     * TEMPORARY TABLESPACE
     */
    
    if ( sTempInd == SQL_NULL_DATA )
    {
        /**
         * User 의 default tablespace 가 DROP 된 경우
         */
    }
    else
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    TEMPORARY TABLESPACE \"%s\" ",
                     aStringDDL,
                     sTempSpace );
    }

    /**
     * WITHOUT SCHEMA
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    WITHOUT SCHEMA ",
                 aStringDDL );
        
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    /**
     * PROFILE
     */
    
    if ( sProfileInd == SQL_NULL_DATA )
    {
        /**
         * User 에게 PROFILE 이 할당되지 않은 경우 
         */
    }
    else
    {
        aStringDDL[0] = '\0';
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\nALTER USER \"%s\" PROFILE \"%s\" ",
                     aStringDDL,
                     sUserName,
                     sProfileName );

        STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}




stlStatus ztqPrintAlterUserSchemaPath( stlInt64        aUserID,
                                       stlChar       * aStringDDL,
                                       stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sUserName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sUserInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;

    stlInt32    sRowCount = 0;
    
    /************************************************************
     * SCHEMA PATH 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       usr.AUTHORIZATION_NAME "
                 "     , sch.SCHEMA_NAME "
                 "  FROM "
                 "       DEFINITION_SCHEMA.USER_SCHEMA_PATH AS upath "
                 "     , DEFINITION_SCHEMA.SCHEMATA         AS sch "
                 "     , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr "
                 " WHERE "
                 "       upath.SCHEMA_ID = sch.SCHEMA_ID "
                 "   AND upath.AUTH_ID   = usr.AUTH_ID "
                 "   AND usr.AUTH_ID     = %ld "
                 " ORDER BY "
                 "        upath.SEARCH_ORDER ",
                 aUserID );
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sUserName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sUserInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * ALTER USER 구문 생성
     ************************************************************/

    sRowCount = 0;
    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }
        else
        {
            sRowCount++;
        }
        
        if ( sRowCount == 1 )
        {
            /**
             * SET SESSION AUTHORIZATION 
             */
            
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "\nSET SESSION AUTHORIZATION \"SYS\"; " );
    
            
            /**
             * ALTER USER
             */
    
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\nALTER USER \"%s\" "
                         "\n    SCHEMA PATH "
                         "\n    ( ",
                         aStringDDL,
                         sUserName );
    
            /**
             * SCHEMA PATH
             */
    
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n        \"%s\" ",
                         aStringDDL,
                         sSchemaName );
        }
        else
        {
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n      , \"%s\" ",
                         aStringDDL,
                         sSchemaName );
        }
    }

    if ( sRowCount > 0 )
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    ) ",
                     aStringDDL );
        
        STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
    }

    /************************************************************
     * Statement 정리
     ************************************************************/

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintOwnerCreateSchema( stlInt64        aOwnerID,
                                     stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSchemaID;
    SQLLEN      sSchemaInd;
    
    /************************************************************
     * SCHEMA 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT SCHEMA_ID "
                 "  FROM DEFINITION_SCHEMA.SCHEMATA "
                 " WHERE OWNER_ID = %ld "
                 " ORDER BY SCHEMA_ID ",
                 aOwnerID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSchemaID,
                            STL_SIZEOF(sSchemaID),
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * SchemaID 에 해당하는 CREATE SCHEMA 구문 출력
         */

        STL_TRY( ztqPrintCreateSchema( sSchemaID,
                                       aStringDDL,
                                       aAllocator,
                                       aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintOwnerCreateTable( stlInt64        aOwnerID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sTableID;
    SQLLEN      sTableInd;
    
    /************************************************************
     * TABLE 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT TABLE_ID "
                 "  FROM DEFINITION_SCHEMA.TABLES "
                 " WHERE TABLE_TYPE = 'BASE TABLE' "
                 "   AND OWNER_ID = %ld "
                 " ORDER BY SCHEMA_ID, TABLE_ID ",
                 aOwnerID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sTableID,
                            STL_SIZEOF(sTableID),
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * TableID 에 해당하는 CREATE TABLE 구문 출력
         */

        STL_TRY( ztqPrintCreateTable( sTableID,
                                      aStringDDL,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintOwnerConstraint( stlInt64        aOwnerID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sConstID;
    SQLLEN      sConstInd;
    
    /************************************************************
     * CONSTRAINT 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT CONSTRAINT_ID "
                 "  FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS "
                 " WHERE CONSTRAINT_TYPE IN ( 'NOT NULL', 'PRIMARY KEY', 'UNIQUE' ) "
                 "   AND CONSTRAINT_OWNER_ID = %ld "
                 " ORDER BY "
                 "       TABLE_SCHEMA_ID "
                 "     , TABLE_ID "
                 "     , CASE WHEN CONSTRAINT_TYPE = 'NOT NULL'    THEN 1 "
                 "            WHEN CONSTRAINT_TYPE = 'PRIMARY KEY' THEN 2 "
                 "            WHEN CONSTRAINT_TYPE = 'UNIQUE'      THEN 3 "
                 "            ELSE NULL "
                 "            END "
                 "     , CONSTRAINT_ID ",
                 aOwnerID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sConstID,
                            STL_SIZEOF(sConstID),
                            & sConstInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        STL_TRY( ztqPrintAddConstraint( sConstID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintOwnerCreateIndex( stlInt64        aOwnerID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sIndexID;
    SQLLEN      sIndexInd;
    
    /************************************************************
     * INDEX 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT idx.INDEX_ID "
                 "  FROM DEFINITION_SCHEMA.INDEXES idx"
                 "     , DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE ikey"
                 " WHERE idx.INDEX_ID = ikey.INDEX_ID "
                 "   AND idx.BY_CONSTRAINT = FALSE "
                 "   AND idx.OWNER_ID = %ld "
                 " ORDER BY "
                 "       ikey.TABLE_SCHEMA_ID "
                 "     , ikey.TABLE_ID "
                 "     , ikey.INDEX_ID ",
                 aOwnerID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sIndexID,
                            STL_SIZEOF(sIndexID),
                            & sIndexInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * IndexID 에 해당하는 CREATE INDEX 구문 출력
         */

        STL_TRY( ztqPrintCreateIndex( sIndexID,
                                      aStringDDL,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintOwnerCreateView( stlInt64        aOwnerID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sViewID;
    SQLLEN      sViewInd;
    
    /************************************************************
     * CREATE VIEW 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT TABLE_ID "
                 "  FROM DEFINITION_SCHEMA.TABLES "
                 " WHERE TABLE_TYPE = 'VIEW' "
                 "   AND OWNER_ID = %ld "
                 " ORDER BY SCHEMA_ID, TABLE_ID ",
                 aOwnerID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sViewID,
                            STL_SIZEOF(sViewID),
                            & sViewInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * ViewID 에 해당하는 CREATE VIEW 구문 출력
         */

        STL_TRY( ztqPrintCreateView( sViewID,
                                     aStringDDL,
                                     aAllocator,
                                     aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * ALTER VIEW 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT TABLE_ID "
                 "  FROM DEFINITION_SCHEMA.TABLES "
                 " WHERE TABLE_TYPE = 'VIEW' "
                 "   AND OWNER_ID = %ld "
                 " ORDER BY SCHEMA_ID, TABLE_ID ",
                 aOwnerID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sViewID,
                            STL_SIZEOF(sViewID),
                            & sViewInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * ViewID 에 해당하는 ALTER VIEW 구문 출력
         */

        STL_TRY( ztqPrintViewRecompile( sViewID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintOwnerCreateSequence( stlInt64        aOwnerID,
                                       stlChar       * aStringDDL,
                                       stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSeqID;
    SQLLEN      sSeqInd;
    
    /************************************************************
     * SEQUENCE 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT SEQUENCE_ID "
                 "  FROM DEFINITION_SCHEMA.SEQUENCES "
                 " WHERE OWNER_ID = %ld "
                 " ORDER BY SCHEMA_ID, SEQUENCE_ID ",
                 aOwnerID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSeqID,
                            STL_SIZEOF(sSeqID),
                            & sSeqInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * SeqID 에 해당하는 CREATE SEQUENCE 구문 출력
         */

        STL_TRY( ztqPrintCreateSequence( sSeqID,
                                         aStringDDL,
                                         aAllocator,
                                         aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintOwnerCreateSynonym( stlInt64        aOwnerID,
                                      stlChar       * aStringDDL,
                                      stlAllocator  * aAllocator,
                                      stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSynonymID;
    SQLLEN      sSynonymInd;
    
    /************************************************************
     * SYNONYM 목록 관련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT SYNONYM_ID "
                 "  FROM DEFINITION_SCHEMA.SYNONYMS "
                 " WHERE OWNER_ID = %ld "
                 " ORDER BY SCHEMA_ID, SYNONYM_ID ",
                 aOwnerID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSynonymID,
                            STL_SIZEOF(sSynonymID),
                            & sSynonymInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * SynonymID 에 해당하는 CREATE SYNONYM 구문 출력
         */

        STL_TRY( ztqPrintCreateSynonym( sSynonymID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintCommentAuth( stlInt64        aAuthID,
                               stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlChar     sAuthName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sAuthInd;
    stlChar     sComment[ZTQ_MAX_COMMENT_LENGTH];
    SQLLEN      sCommentInd;

    /************************************************************
     * COMMENT 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME  "
                 "      , auth.COMMENTS             "
                 "   FROM "
                 "        DEFINITION_SCHEMA.AUTHORIZATIONS        AS auth  "
                 "  WHERE "
                 "        auth.AUTH_ID = %ld ",
                 aAuthID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sAuthName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sAuthInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sComment,
                            ZTQ_MAX_COMMENT_LENGTH,
                            & sCommentInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * COMMENT ON AUTHORIZATION 구문 생성
     ************************************************************/

    if ( sCommentInd == SQL_NULL_DATA )
    {
        /* comment 가 없는 경우 */
    }
    else
    {
        /**
         * SET SESSION AUTHORIZATION owner
         */
    
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "\nSET SESSION AUTHORIZATION \"SYS\"; " );
    
        /**
         * COMMENT
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\nCOMMENT "
                     "\n    ON AUTHORIZATION \"%s\" ",
                     aStringDDL,
                     sAuthName );

        /**
         * IS 'comment'
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    IS '%s' ",
                     aStringDDL,
                     sComment );
    
        STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}





/****************************************************************************
 * Schema
 ****************************************************************************/

stlStatus ztqGetSchemaID( ztqIdentifier * aIdentifier,
                          stlAllocator  * aAllocator,
                          stlInt64      * aSchemaID,
                          stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt32    sLen = 0;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    stlInt64    sSchemaID;
    SQLLEN      sSchemaInd;

    /************************************************************
     * User Name 획득
     ************************************************************/
    
    if( aIdentifier->mIdentifier[0] == '"' )
    {
        sLen = stlStrlen( aIdentifier->mIdentifier );
        stlMemcpy( sSchemaName, aIdentifier->mIdentifier + 1, sLen - 2 );
        sSchemaName[sLen-2] = '\0';
    }
    else
    {
        stlStrcpy( sSchemaName, aIdentifier->mIdentifier );
    }

    /************************************************************
     * USER 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;
    
    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT SCHEMA_ID "
                 "  FROM DEFINITION_SCHEMA.SCHEMATA "
                 " WHERE SCHEMA_NAME = '%s' ",
                 sSchemaName );

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSchemaID,
                            STL_SIZEOF(sSchemaID),
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aSchemaID = sSchemaID;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintCreateSchema( stlInt64        aUserID,
                                stlChar       * aStringDDL,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;

    /************************************************************
     * CREATE SCHEMA 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT sch.SCHEMA_NAME "
                 "     , auth.AUTHORIZATION_NAME "
                 "  FROM DEFINITION_SCHEMA.SCHEMATA sch, "
                 "       DEFINITION_SCHEMA.AUTHORIZATIONS auth "
                 " WHERE sch.OWNER_ID  = auth.AUTH_ID"
                 "   AND sch.SCHEMA_ID = %ld ",
                 aUserID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * CREATE SCHEMA 구문 생성
     ************************************************************/

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"SYS\"; " );
    
    /**
     * CREATE SCHEMA
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nCREATE SCHEMA \"%s\" ",
                 aStringDDL,
                 sSchemaName );
    
    /**
     * AUTHORIZATION
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    AUTHORIZATION \"%s\" ",
                 aStringDDL,
                 sOwnerName );
        
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}




stlStatus ztqPrintSchemaCreateTable( stlInt64        aSchemaID,
                                     stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sTableID;
    SQLLEN      sTableInd;
    
    /************************************************************
     * TABLE 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT TABLE_ID "
                 "  FROM DEFINITION_SCHEMA.TABLES "
                 " WHERE TABLE_TYPE = 'BASE TABLE' "
                 "   AND SCHEMA_ID = %ld "
                 " ORDER BY TABLE_ID ",
                 aSchemaID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sTableID,
                            STL_SIZEOF(sTableID),
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * TableID 에 해당하는 CREATE TABLE 구문 출력
         */

        STL_TRY( ztqPrintCreateTable( sTableID,
                                      aStringDDL,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}




stlStatus ztqPrintSchemaConstraint( stlInt64        aSchemaID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sConstID;
    SQLLEN      sConstInd;
    
    /************************************************************
     * CONSTRAINT 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT CONSTRAINT_ID "
                 "  FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS "
                 " WHERE CONSTRAINT_TYPE IN ( 'NOT NULL', 'PRIMARY KEY', 'UNIQUE' ) "
                 "   AND TABLE_SCHEMA_ID = %ld "
                 " ORDER BY "
                 "       TABLE_ID "
                 "     , CASE WHEN CONSTRAINT_TYPE = 'NOT NULL'    THEN 1 "
                 "            WHEN CONSTRAINT_TYPE = 'PRIMARY KEY' THEN 2 "
                 "            WHEN CONSTRAINT_TYPE = 'UNIQUE'      THEN 3 "
                 "            ELSE NULL "
                 "            END "
                 "     , CONSTRAINT_ID ",
                 aSchemaID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sConstID,
                            STL_SIZEOF(sConstID),
                            & sConstInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        STL_TRY( ztqPrintAddConstraint( sConstID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintSchemaCreateIndex( stlInt64        aSchemaID,
                                     stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sIndexID;
    SQLLEN      sIndexInd;
    
    /************************************************************
     * INDEX 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT idx.INDEX_ID "
                 "  FROM DEFINITION_SCHEMA.INDEXES idx"
                 "     , DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE ikey"
                 " WHERE idx.INDEX_ID = ikey.INDEX_ID "
                 "   AND idx.BY_CONSTRAINT = FALSE "
                 "   AND idx.SCHEMA_ID = %ld "
                 " ORDER BY "
                 "       ikey.TABLE_SCHEMA_ID "
                 "     , ikey.TABLE_ID "
                 "     , ikey.INDEX_ID ",
                 aSchemaID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sIndexID,
                            STL_SIZEOF(sIndexID),
                            & sIndexInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * IndexID 에 해당하는 CREATE INDEX 구문 출력
         */

        STL_TRY( ztqPrintCreateIndex( sIndexID,
                                      aStringDDL,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintSchemaCreateView( stlInt64        aSchemaID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sViewID;
    SQLLEN      sViewInd;
    
    /************************************************************
     * VIEW 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT TABLE_ID "
                 "  FROM DEFINITION_SCHEMA.TABLES "
                 " WHERE TABLE_TYPE = 'VIEW' "
                 "   AND SCHEMA_ID = %ld "
                 " ORDER BY TABLE_ID ",
                 aSchemaID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sViewID,
                            STL_SIZEOF(sViewID),
                            & sViewInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * ViewID 에 해당하는 CREATE VIEW 구문 출력
         */

        STL_TRY( ztqPrintCreateView( sViewID,
                                     aStringDDL,
                                     aAllocator,
                                     aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );


    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * ALTER VIEW 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT TABLE_ID "
                 "  FROM DEFINITION_SCHEMA.TABLES "
                 " WHERE TABLE_TYPE = 'VIEW' "
                 "   AND SCHEMA_ID = %ld "
                 " ORDER BY TABLE_ID ",
                 aSchemaID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sViewID,
                            STL_SIZEOF(sViewID),
                            & sViewInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * ViewID 에 해당하는 ALTER VIEW 구문 출력
         */

        STL_TRY( ztqPrintViewRecompile( sViewID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );


    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintSchemaCreateSequence( stlInt64        aSchemaID,
                                        stlChar       * aStringDDL,
                                        stlAllocator  * aAllocator,
                                        stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSeqID;
    SQLLEN      sSeqInd;
    
    /************************************************************
     * SEQUENCE 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT SEQUENCE_ID "
                 "  FROM DEFINITION_SCHEMA.SEQUENCES "
                 " WHERE SCHEMA_ID = %ld "
                 " ORDER BY SEQUENCE_ID ",
                 aSchemaID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSeqID,
                            STL_SIZEOF(sSeqID),
                            & sSeqInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * SeqID 에 해당하는 CREATE SEQUENCE 구문 출력
         */

        STL_TRY( ztqPrintCreateSequence( sSeqID,
                                         aStringDDL,
                                         aAllocator,
                                         aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintSchemaCreateSynonym( stlInt64        aSchemaID,
                                       stlChar       * aStringDDL,
                                       stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSynonymID;
    SQLLEN      sSynonymInd;
    
    /************************************************************
     * SYNONYM 목록 관련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT SYNONYM_ID "
                 "  FROM DEFINITION_SCHEMA.SYNONYMS "
                 " WHERE SCHEMA_ID = %ld "
                 " ORDER BY SYNONYM_ID ",
                 aSchemaID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSynonymID,
                            STL_SIZEOF(sSynonymID),
                            & sSynonymInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * SynonymID 에 해당하는 CREATE SYNONYM 구문 출력
         */

        STL_TRY( ztqPrintCreateSynonym( sSynonymID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintGrantSchema( stlInt64        aSchemaID,
                               stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlRegionMark   sMark;
    stlBool         sIsMarked = STL_FALSE;

    ztqPrivOrder  sPrivOrder;
    
    stlInt64    sOrgPrivType;
    stlChar     sOrgPrivName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    stlInt64    sOwnerID;
    SQLLEN      sOwnerInd;

    stlInt64    sGrantorID;
    SQLLEN      sGrantorInd;
    stlChar     sGrantorName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGrantorNameInd;
    
    stlInt64    sGranteeID;
    SQLLEN      sGranteeInd;
    stlChar     sGranteeName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGranteeNameInd;
    stlChar     sIsBuiltIn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsBuiltInInd;

    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    
    stlInt64    sPrivType;
    SQLLEN      sPrivTypeInd;
    stlChar     sPrivName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sPrivNameInd;
    
    stlChar     sGrantable[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGrantInd;

    /**
     * Memory Mark 설정
     */
    
    STL_TRY( stlMarkRegionMem( aAllocator,
                               &sMark,
                               aErrorStack ) == STL_SUCCESS );
    sIsMarked = STL_TRUE;
    
    /************************************************************
     * GRANT priv ON SCHEMA 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       ( SELECT AUTH_ID "
                 "           FROM DEFINITION_SCHEMA.AUTHORIZATIONS "
                 "          WHERE AUTHORIZATION_NAME = '_SYSTEM' ) "
                 "     , grantor.AUTH_ID "
                 "     , grantor.AUTHORIZATION_NAME "
                 "     , grantee.AUTH_ID "
                 "     , grantee.AUTHORIZATION_NAME "
                 "     , CAST( CASE WHEN priv.IS_BUILTIN = TRUE "
                 "                  THEN 'YES' "
                 "                  ELSE 'NO' "
                 "                  END AS VARCHAR(3 OCTETS) ) "
                 "     , sch.SCHEMA_NAME "
                 "     , priv.PRIVILEGE_TYPE_ID "
                 "     , priv.PRIVILEGE_TYPE "
                 "     , CAST( CASE WHEN priv.IS_GRANTABLE = TRUE THEN 'YES' "
                 "                                                ELSE 'NO' "
                 "             END AS VARCHAR(3 OCTETS) ) "
                 "  FROM "
                 "       DEFINITION_SCHEMA.SCHEMA_PRIVILEGES  AS priv "
                 "     , DEFINITION_SCHEMA.SCHEMATA           AS sch "
                 "     , DEFINITION_SCHEMA.AUTHORIZATIONS     AS grantor "
                 "     , DEFINITION_SCHEMA.AUTHORIZATIONS     AS grantee "
                 " WHERE "
                 "       sch.SCHEMA_ID    = priv.SCHEMA_ID "
                 "   AND priv.GRANTOR_ID  = grantor.AUTH_ID "
                 "   AND priv.GRANTEE_ID  = grantee.AUTH_ID "
                 "   AND sch.SCHEMA_ID    = %ld "
                 " ORDER BY "
                 "       priv.PRIVILEGE_TYPE_ID "
                 "     , grantor.AUTH_ID "
                 "     , grantee.AUTH_ID ",
                 aSchemaID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sOwnerID,
                            STL_SIZEOF(sOwnerID),
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_SBIGINT,
                            & sGrantorID,
                            STL_SIZEOF(sGrantorID),
                            & sGrantorInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sGrantorName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGrantorNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_SBIGINT,
                            & sGranteeID,
                            STL_SIZEOF(sGranteeID),
                            & sGranteeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_CHAR,
                            sGranteeName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGranteeNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            6,
                            SQL_C_CHAR,
                            sIsBuiltIn,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsBuiltInInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            7,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            8,
                            SQL_C_SBIGINT,
                            & sPrivType,
                            STL_SIZEOF(sPrivType),
                            & sPrivTypeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            9,
                            SQL_C_CHAR,
                            sPrivName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sPrivNameInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            10,
                            SQL_C_CHAR,
                            sGrantable,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGrantInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    /**
     * 최초 Fetch
     */
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_FINISH );

    /**
     * Privilege Print Order 결정을 위한 초기화
     */
    
    sOrgPrivType = sPrivType;
    stlStrncpy( sOrgPrivName, sPrivName, STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    ztqInitPrivOrder( & sPrivOrder );

    STL_TRY( ztqAddPrivItem( & sPrivOrder,
                             sGrantorID,
                             sGrantorName,
                             sGranteeID,
                             sGranteeName,
                             sIsBuiltIn,
                             sGrantable,
                             aAllocator,
                             aErrorStack )
             == STL_SUCCESS );
    
    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            ztqBuildPrivOrder( & sPrivOrder, sOwnerID );
            
            STL_TRY( ztqPrintGrantByPrivOrder( aStringDDL,
                                               & sPrivOrder,
                                               ZTQ_PRIV_OBJECT_SCHEMA,
                                               sOrgPrivName,
                                               sSchemaName, 
                                               NULL, /* aObjectName */
                                               NULL, /* aColumnName */
                                               aErrorStack )
                     == STL_SUCCESS );
            break;
        }

        if ( sPrivType == sOrgPrivType )
        {
            /**
             * 이전과 동일한 Privilege Type 인 경우
             */

            STL_TRY( ztqAddPrivItem( & sPrivOrder,
                                     sGrantorID,
                                     sGrantorName,
                                     sGranteeID,
                                     sGranteeName,
                                     sIsBuiltIn,
                                     sGrantable,
                                     aAllocator,
                                     aErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * 서로 다른 Privilege Type 인 경우
             */

            ztqBuildPrivOrder( & sPrivOrder, sOwnerID );
            
            STL_TRY( ztqPrintGrantByPrivOrder( aStringDDL,
                                               & sPrivOrder,
                                               ZTQ_PRIV_OBJECT_SCHEMA,
                                               sOrgPrivName,
                                               sSchemaName, 
                                               NULL, /* aObjectName */
                                               NULL, /* aColumnName */
                                               aErrorStack )
                     == STL_SUCCESS );

            /**
             * 메모리 재사용
             */
            
            sIsMarked = STL_FALSE;
            STL_TRY( stlRestoreRegionMem( aAllocator,
                                          &sMark,
                                          STL_FALSE, /* aFree */
                                          aErrorStack ) == STL_SUCCESS );
            STL_TRY( stlMarkRegionMem( aAllocator,
                                       &sMark,
                                       aErrorStack ) == STL_SUCCESS );
            sIsMarked = STL_TRUE;

            /**
             * Privilege Print Order 결정을 위한 초기화
             */
            
            sOrgPrivType = sPrivType;
            stlStrncpy( sOrgPrivName, sPrivName, STL_MAX_SQL_IDENTIFIER_LENGTH );
            
            ztqInitPrivOrder( & sPrivOrder );
            
            STL_TRY( ztqAddPrivItem( & sPrivOrder,
                                     sGrantorID,
                                     sGrantorName,
                                     sGranteeID,
                                     sGranteeName,
                                     sIsBuiltIn,
                                     sGrantable,
                                     aAllocator,
                                     aErrorStack )
                     == STL_SUCCESS );
        }
    }

    /************************************************************
     * 마무리 
     ************************************************************/

    STL_RAMP( RAMP_FINISH );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /**
     * Memory Mark 해제
     */
    
    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        STL_TRY( stlRestoreRegionMem( aAllocator,
                                      &sMark,
                                      STL_FALSE, /* aFree */
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        (void) stlRestoreRegionMem( aAllocator,
                                    &sMark,
                                    STL_FALSE, /* aFree */
                                    aErrorStack );
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintCommentSchema( stlInt64        aSchemaID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sComment[ZTQ_MAX_COMMENT_LENGTH];
    SQLLEN      sCommentInd;

    /************************************************************
     * COMMENT 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME  "
                 "      , sch.SCHEMA_NAME          "
                 "      , sch.COMMENTS             "
                 "   FROM "
                 "        DEFINITION_SCHEMA.SCHEMATA              AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS        AS auth  "
                 "  WHERE "
                 "        sch.OWNER_ID    = auth.AUTH_ID "
                 "    AND sch.SCHEMA_ID    = %ld ",
                 aSchemaID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sComment,
                            ZTQ_MAX_COMMENT_LENGTH,
                            & sCommentInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * COMMENT ON SCHEMA 구문 생성
     ************************************************************/

    if ( sCommentInd == SQL_NULL_DATA )
    {
        /* comment 가 없는 경우 */
    }
    else
    {
        /**
         * SET SESSION AUTHORIZATION owner
         */
    
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "\nSET SESSION AUTHORIZATION \"%s\"; ",
                     sOwnerName );
    
        /**
         * COMMENT
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\nCOMMENT "
                     "\n    ON SCHEMA \"%s\" ",
                     aStringDDL,
                     sSchemaName );

        /**
         * IS 'comment'
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    IS '%s' ",
                     aStringDDL,
                     sComment );
    
        STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


/****************************************************************************
 * Public Synonym
 ****************************************************************************/


stlStatus ztqGetPublicSynonymID( ztqIdentifier * aIdentifier,
                                 stlAllocator  * aAllocator,
                                 stlInt64      * aSynonymID,
                                 stlErrorStack * aErrorStack )
{
    stlInt32   sLen = 0;

    stlBool    sIsQualified = STL_FALSE;
    stlChar    sSynonymName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    /************************************************************
     * double-quoted identifier 처리 
     ************************************************************/

    if( aIdentifier->mIdentifier[0] == '"' )
    {
        sLen = stlStrlen( aIdentifier->mIdentifier );
        stlMemcpy( sSynonymName, aIdentifier->mIdentifier + 1, sLen - 2 );
        sSynonymName[sLen-2] = '\0';
    }
    else
    {
        stlStrcpy( sSynonymName, aIdentifier->mIdentifier );
    }

    /************************************************************
     * Public Synonym ID 획득
     ************************************************************/

    if ( sIsQualified == STL_TRUE )
    {
        STL_TRY( ztqGetQualifiedPublicSynonymID( sSynonymName,
                                                 aAllocator,
                                                 aSynonymID,
                                                 aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztqGetNonQualifiedPublicSynonymID( sSynonymName,
                                                    aAllocator,
                                                    aSynonymID,
                                                    aErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqGetQualifiedPublicSynonymID( stlChar       * aSynonymName,
                                          stlAllocator  * aAllocator,
                                          stlInt64      * aSynonymID,
                                          stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSynonymID;
    SQLLEN      sSynonymInd;
    
    /************************************************************
     * SYNONYM ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT psyn.SYNONYM_ID "
                 "  FROM DEFINITION_SCHEMA.PUBLIC_SYNONYMS psyn "
                 " WHERE psyn.SYNONYM_NAME = '%s' ",
                 aSynonymName );

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSynonymID,
                            STL_SIZEOF(sSynonymID),
                            & sSynonymInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aSynonymID = sSynonymID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqGetNonQualifiedPublicSynonymID( stlChar       * aSynonymName,
                                             stlAllocator  * aAllocator,
                                             stlInt64      * aSynonymID,
                                             stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSynonymID;
    SQLLEN      sSynonymInd;
    
    /************************************************************
     * SYNONYM ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        psyn.SYNONYM_ID "
                 "   FROM "
                 "        DEFINITION_SCHEMA.PUBLIC_SYNONYMS         AS psyn "
                 " WHERE psyn.SYNONYM_NAME = '%s' "
                 " FETCH 1 ",
                 aSynonymName );
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSynonymID,
                            STL_SIZEOF(sSynonymID),
                            & sSynonymInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aSynonymID = sSynonymID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintCreatePublicSynonym( stlInt64        aSynonymID,
                                       stlChar       * aStringDDL,
                                       stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlChar     sSynonymName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSynonymInd;

    stlChar     sObjectSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sObjectSchemaNameInd;
    stlChar     sObjectName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sObjectNameInd;
    
    /************************************************************
     * CREATE PUBLIC SYNONYM 관련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        psyn.SYNONYM_NAME         "
                 "      , psyn.OBJECT_SCHEMA_NAME   "
                 "      , psyn.OBJECT_NAME          "
                 "   FROM "
                 "        DEFINITION_SCHEMA.PUBLIC_SYNONYMS       AS psyn "
                 "  WHERE "
                 "        psyn.SYNONYM_ID = %ld ",
                 aSynonymID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sSynonymName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSynonymInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sObjectSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sObjectSchemaNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sObjectName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sObjectNameInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * CREATE PUBLIC SYNONYM 구문 생성
     ************************************************************/

    /**
     * SET SESSION AUTHORIZATION grantor
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"SYS\"; " );
    
    /**
     * CREATE PUBLIC SYNONYM
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nCREATE PUBLIC SYNONYM \"%s\" FOR \"%s\".\"%s\"",
                 aStringDDL,
                 sSynonymName,
                 sObjectSchemaName,
                 sObjectName );
    
    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


/****************************************************************************
 * Table
 ****************************************************************************/


stlStatus ztqGetTableID( ztqIdentifier * aIdentifierList,
                         stlAllocator  * aAllocator,
                         stlInt64      * aTableID,
                         stlErrorStack * aErrorStack )
{
    stlChar  * sIdentifier = NULL;
    stlInt32   sLen = 0;

    stlBool     sIsQualified = STL_FALSE;
    stlChar     sTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    /************************************************************
     * double-quoted identifier 처리 
     ************************************************************/

    if ( aIdentifierList->mNext == NULL )
    {
        /**
         * table_name 만 기술한 경우 
         */

        sIsQualified = STL_FALSE;
        sIdentifier = aIdentifierList->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sTableName, sIdentifier + 1, sLen - 2 );
            sTableName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sTableName, sIdentifier );
        }
    }
    else
    {
        /**
         * schema_name.table_name 으로  기술한 경우 
         */
        
        sIsQualified = STL_TRUE;

        sIdentifier = aIdentifierList->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sSchemaName, sIdentifier + 1, sLen - 2 );
            sSchemaName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sSchemaName, sIdentifier );
        }

        sIdentifier = ((ztqIdentifier *) aIdentifierList->mNext)->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sTableName, sIdentifier + 1, sLen - 2 );
            sTableName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sTableName, sIdentifier );
        }
    }

    /************************************************************
     * Table ID 획득
     ************************************************************/

    if ( sIsQualified == STL_TRUE )
    {
        STL_TRY( ztqGetQualifiedTableID( sSchemaName,
                                         sTableName,
                                         aAllocator,
                                         aTableID,
                                         aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztqGetNonQualifiedTableID( sTableName,
                                            aAllocator,
                                            aTableID,
                                            aErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqGetQualifiedTableID( stlChar       * aSchemaName,
                                  stlChar       * aTableName,
                                  stlAllocator  * aAllocator,
                                  stlInt64      * aTableID,
                                  stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sTableID;
    SQLLEN      sTableInd;
    
    /************************************************************
     * TABLE ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT tab.TABLE_ID "
                 "  FROM DEFINITION_SCHEMA.TABLES tab "
                 "     , DEFINITION_SCHEMA.SCHEMATA sch "
                 " WHERE tab.SCHEMA_ID = sch.SCHEMA_ID "
                 "   AND tab.TABLE_TYPE = 'BASE TABLE' "
                 "   AND tab.TABLE_NAME = '%s' "
                 "   AND sch.SCHEMA_NAME = '%s' ",
                 aTableName,
                 aSchemaName );

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sTableID,
                            STL_SIZEOF(sTableID),
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aTableID = sTableID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqGetNonQualifiedTableID( stlChar       * aTableName,
                                     stlAllocator  * aAllocator,
                                     stlInt64      * aTableID,
                                     stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sTableID;
    SQLLEN      sTableInd;
    
    /************************************************************
     * TABLE ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        tab.TABLE_ID "
                 "   FROM "
                 "        DEFINITION_SCHEMA.SCHEMATA         AS sch "
                 "      , DEFINITION_SCHEMA.TABLES           AS tab "
                 "      , ( "
                 "         ( SELECT "
                 "                sch1.SCHEMA_ID  "
                 "              , path1.SEARCH_ORDER  "
                 "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path1 "
                 "              , DEFINITION_SCHEMA.SCHEMATA         AS sch1 "
                 "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr1 "
                 "          WHERE "
                 "                usr1.AUTHORIZATION_NAME = CURRENT_USER "
                 "            AND path1.SCHEMA_ID         = sch1.SCHEMA_ID "
                 "            AND path1.AUTH_ID           = usr1.AUTH_ID "
                 "          ORDER BY "
                 "                path1.SEARCH_ORDER "
                 "         ) "
                 "       UNION ALL "
                 "         ( SELECT "
                 "                sch2.SCHEMA_ID "
                 "              , ( SELECT COUNT(*) "
                 "                    FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH "
                 "                   WHERE AUTH_ID = USER_ID() ) "
                 "                + path2.SEARCH_ORDER "
                 "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path2 "
                 "              , DEFINITION_SCHEMA.SCHEMATA         AS sch2  "
                 "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr2 "
                 "          WHERE "
                 "                usr2.AUTHORIZATION_NAME = 'PUBLIC' "
                 "            AND path2.SCHEMA_ID         = sch2.SCHEMA_ID "
                 "            AND path2.AUTH_ID           = usr2.AUTH_ID "
                 "          ORDER BY "
                 "                path2.SEARCH_ORDER "
                 "         ) "
                 "       ) AS upath ( SCHEMA_ID, SEARCH_ORDER ) "
                 " WHERE tab.SCHEMA_ID = sch.SCHEMA_ID "
                 "   AND sch.SCHEMA_ID = upath.SCHEMA_ID "
                 "   AND tab.TABLE_TYPE = 'BASE TABLE' "
                 "   AND tab.TABLE_NAME = '%s' "
                 " ORDER BY "
                 "       upath.SEARCH_ORDER  "
                 " FETCH 1 ",
                 aTableName );
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sTableID,
                            STL_SIZEOF(sTableID),
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aTableID = sTableID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintCreateTable( stlInt64        aTableID,
                               stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sTableInd;
    stlChar     sSpaceName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSpaceInd;

    stlChar     sCacheTableType[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sCacheTableTypeInd;
    
    stlInt64    sPctFree;
    SQLLEN      sPctFreeInd;
    stlInt64    sPctUsed;
    SQLLEN      sPctUsedInd;
    stlInt64    sIniTrans;
    SQLLEN      sIniTransInd;
    stlInt64    sMaxTrans;
    SQLLEN      sMaxTransInd;
    stlInt64    sSegInit;
    SQLLEN      sSegInitInd;
    stlInt64    sSegNext;
    SQLLEN      sSegNextInd;
    stlInt64    sSegMin;
    SQLLEN      sSegMinInd;
    stlInt64    sSegMax;
    SQLLEN      sSegMaxInd;
    

    stlInt32    sColCount = 0;
    stlChar     sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sColumnInd;

    stlInt64    sTypeID;
    SQLLEN      sTypeInd;

    stlInt64    sStringMaxLen;
    SQLLEN      sStringMaxLenInd;
    stlInt64    sLengthUnit;
    SQLLEN      sLengthUnitInd;

    stlInt64    sNumPrec;
    SQLLEN      sNumPrecInd;
    stlInt64    sNumScale;
    SQLLEN      sNumScaleInd;

    stlInt64    sTimePrec;
    SQLLEN      sTimePrecInd;

    stlInt64    sIntervalType;
    SQLLEN      sIntervalTypeInd;
    stlInt64    sIntervalPrec;
    SQLLEN      sIntervalPrecInd;
    
    stlChar   * sDefaultLong;
    SQLLEN      sDefaultInd;
    
    stlChar     sIsIdentity[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsIdentityInd;
    stlChar     sIdentityGen[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIdentityGenInd;
    stlInt64    sIdentityStart;
    SQLLEN      sIdentityStartInd;
    stlInt64    sIdentityInc;
    SQLLEN      sIdentityIncInd;
    stlInt64    sIdentityMax;
    SQLLEN      sIdentityMaxInd;
    stlInt64    sIdentityMin;
    SQLLEN      sIdentityMinInd;
    stlChar     sIsCycle[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsCycleInd;
    stlInt64    sIdentityCache;
    SQLLEN      sIdentityCacheInd;
    
    stlRegionMark   sMark;
    stlBool         sIsMarked = STL_FALSE;
    
    /**
     * DEFAULT 공간 확보
     */

    STL_TRY( stlMarkRegionMem( aAllocator,
                               &sMark,
                               aErrorStack ) == STL_SUCCESS );

    sIsMarked = STL_TRUE;
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sDefaultLong,
                                aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * CREATE TABLE 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME   "
                 "      , sch.SCHEMA_NAME           "
                 "      , tab.TABLE_NAME            "
                 "      , spc.TABLESPACE_NAME       "
                 "      , xtcac.TABLE_TYPE          "
                 "      , xtcac.PCTFREE             "
                 "      , xtcac.PCTUSED             "
                 "      , xtcac.INITRANS            "
                 "      , xtcac.MAXTRANS            "
                 "      , ( xseg.INITIAL_EXTENTS * xspc.EXTSIZE ) "
                 "      , ( xseg.NEXT_EXTENTS * xspc.EXTSIZE )    "
                 "      , ( xseg.MIN_EXTENTS * xspc.EXTSIZE )     "
                 "      , ( xseg.MAX_EXTENTS * xspc.EXTSIZE )     "
                 "   FROM "
                 "        DEFINITION_SCHEMA.TABLES           AS tab  "
                 "      , DEFINITION_SCHEMA.TABLESPACES      AS spc   "
                 "      , FIXED_TABLE_SCHEMA.X$TABLESPACE    AS xspc   "
                 "      , FIXED_TABLE_SCHEMA.X$TABLE_CACHE   AS xtcac "
                 "      , FIXED_TABLE_SCHEMA.X$SEGMENT       AS xseg "
                 "      , DEFINITION_SCHEMA.SCHEMATA         AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS   AS auth  "
                 "  WHERE "
                 "        tab.TABLESPACE_ID = spc.TABLESPACE_ID "
                 "    AND tab.PHYSICAL_ID   = xtcac.PHYSICAL_ID "
                 "    AND tab.PHYSICAL_ID   = xseg.PHYSICAL_ID "
                 "    AND tab.SCHEMA_ID     = sch.SCHEMA_ID "
                 "    AND tab.OWNER_ID      = auth.AUTH_ID "
                 "    AND tab.TABLESPACE_ID = xspc.ID "
                 "    AND tab.TABLE_ID      = %ld ",
                 aTableID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sTableName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sSpaceName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSpaceInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_CHAR,
                            sCacheTableType,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sCacheTableTypeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            6,
                            SQL_C_SBIGINT,
                            & sPctFree,
                            STL_SIZEOF(sPctFree),
                            & sPctFreeInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            7,
                            SQL_C_SBIGINT,
                            & sPctUsed,
                            STL_SIZEOF(sPctUsed),
                            & sPctUsedInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            8,
                            SQL_C_SBIGINT,
                            & sIniTrans,
                            STL_SIZEOF(sIniTrans),
                            & sIniTransInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            9,
                            SQL_C_SBIGINT,
                            & sMaxTrans,
                            STL_SIZEOF(sMaxTrans),
                            & sMaxTransInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            10,
                            SQL_C_SBIGINT,
                            & sSegInit,
                            STL_SIZEOF(sSegInit),
                            & sSegInitInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            11,
                            SQL_C_SBIGINT,
                            & sSegNext,
                            STL_SIZEOF(sSegNext),
                            & sSegNextInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            12,
                            SQL_C_SBIGINT,
                            & sSegMin,
                            STL_SIZEOF(sSegMin),
                            & sSegMinInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            13,
                            SQL_C_SBIGINT,
                            & sSegMax,
                            STL_SIZEOF(sSegMax),
                            & sSegMaxInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * CREATE TABLE 구문의 Header 생성
     ************************************************************/

    /**
     * SET SESSION AUTHORIZATION grantor
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"%s\"; ",
                 sOwnerName );
    
    /**
     * CREATE TABLE
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nCREATE TABLE \"%s\".\"%s\" ",
                 aStringDDL,
                 sSchemaName,
                 sTableName );

    /************************************************************
     * Column 정보 생성
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;


    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        col.COLUMN_NAME "
                 "      , dtd.DATA_TYPE_ID "
                 "      , dtd.STRING_LENGTH_UNIT_ID "
                 "      , dtd.CHARACTER_MAXIMUM_LENGTH "
                 "      , dtd.NUMERIC_PRECISION "
                 "      , dtd.NUMERIC_SCALE "
                 "      , dtd.DATETIME_PRECISION "
                 "      , dtd.INTERVAL_TYPE_ID "
                 "      , dtd.INTERVAL_PRECISION "
                 "      , RTRIM( col.COLUMN_DEFAULT ) "
                 "      , CAST( CASE WHEN col.IS_IDENTITY = TRUE "
                 "                   THEN 'YES' "
                 "                   ELSE 'NO' "
                 "                   END " 
                 "              AS VARCHAR(3 OCTETS) ) "
                 "      , col.IDENTITY_GENERATION "
                 "      , col.IDENTITY_START "
                 "      , col.IDENTITY_INCREMENT "
                 "      , col.IDENTITY_MAXIMUM "
                 "      , col.IDENTITY_MINIMUM "
                 "      , CASE WHEN col.IDENTITY_CYCLE = TRUE  "
                 "             THEN 'Y' "
                 "             ELSE 'N' "
                 "             END "                
                 "      , col.IDENTITY_CACHE_SIZE "
                 "   FROM   "
                 "        DEFINITION_SCHEMA.COLUMNS AS col "
                 "      , DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR AS dtd "
                 "      , DEFINITION_SCHEMA.TABLES  AS tab  "
                 "  WHERE  "
                 "        col.COLUMN_ID = dtd.COLUMN_ID "
                 "    AND col.TABLE_ID  = tab.TABLE_ID "
                 "    AND col.IS_UNUSED = FALSE "
                 "    AND col.TABLE_ID  = %ld "
                 " ORDER BY  "
                 "        col.PHYSICAL_ORDINAL_POSITION ",
                 aTableID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sColumnName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sColumnInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_SBIGINT,
                            & sTypeID,
                            STL_SIZEOF(sTypeID),
                            & sTypeInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_SBIGINT,
                            & sLengthUnit,
                            STL_SIZEOF(sLengthUnit),
                            & sLengthUnitInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_SBIGINT,
                            & sStringMaxLen,
                            STL_SIZEOF(sStringMaxLen),
                            & sStringMaxLenInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_SBIGINT,
                            & sNumPrec,
                            STL_SIZEOF(sNumPrec),
                            & sNumPrecInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            6,
                            SQL_C_SBIGINT,
                            & sNumScale,
                            STL_SIZEOF(sNumScale),
                            & sNumScaleInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            7,
                            SQL_C_SBIGINT,
                            & sTimePrec,
                            STL_SIZEOF(sTimePrec),
                            & sTimePrecInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            8,
                            SQL_C_SBIGINT,
                            & sIntervalType,
                            STL_SIZEOF(sIntervalType),
                            & sIntervalTypeInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            9,
                            SQL_C_SBIGINT,
                            & sIntervalPrec,
                            STL_SIZEOF(sIntervalPrec),
                            & sIntervalPrecInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            10,
                            SQL_C_CHAR,
                            sDefaultLong,
                            gZtqDdlSize,
                            & sDefaultInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            11,
                            SQL_C_CHAR,
                            sIsIdentity,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsIdentityInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            12,
                            SQL_C_CHAR,
                            sIdentityGen,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIdentityGenInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            13,
                            SQL_C_SBIGINT,
                            & sIdentityStart,
                            STL_SIZEOF(sIdentityStart),
                            & sIdentityStartInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            14,
                            SQL_C_SBIGINT,
                            & sIdentityInc,
                            STL_SIZEOF(sIdentityInc),
                            & sIdentityIncInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            15,
                            SQL_C_SBIGINT,
                            & sIdentityMax,
                            STL_SIZEOF(sIdentityMax),
                            & sIdentityMaxInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            16,
                            SQL_C_SBIGINT,
                            & sIdentityMin,
                            STL_SIZEOF(sIdentityMin),
                            & sIdentityMinInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            17,
                            SQL_C_CHAR,
                            sIsCycle,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsCycleInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            18,
                            SQL_C_SBIGINT,
                            & sIdentityCache,
                            STL_SIZEOF(sIdentityCache),
                            & sIdentityCacheInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    /**
     * column 시작
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    ( ",
                 aStringDDL );

    sColCount = 0;
    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        if ( sResult == SQL_SUCCESS_WITH_INFO )
        {
            /**
             * DEFAULT 값이 Buffer 공간을 초과한 경우 character length of data greater than column length
             * - 그 외의 경우라도 문제를 유발할 수 있으므로, 정확한 ERROR_CODE 검사 없이 에러를 낸다.
             */
            if ( sDefaultInd >= gZtqDdlSize )
            {
                STL_THROW( RAMP_ERR_NOT_ENOUGH_DDLSIZE );
            }
            else
            {
                /**
                 * ztqSQLFetch() 에서 메시지를 출력함
                 */
                STL_TRY( 0 );
            }
        }

        /**
         * Column 이름
         */
        
        if ( sColCount == 0 )
        {
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n        \"%s\"",
                         aStringDDL,
                         sColumnName );
        }
        else
        {
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n      , \"%s\"",
                         aStringDDL,
                         sColumnName );
        }

        /**
         * Data Type
         */
        
        switch (sTypeID)
        {
            case DTL_TYPE_BOOLEAN:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID] );
                    
                    break;
                }
            case DTL_TYPE_NATIVE_SMALLINT:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID] );
                    break;
                }
            case DTL_TYPE_NATIVE_INTEGER:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID] );
                    break;
                }
            case DTL_TYPE_NATIVE_BIGINT:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID] );
                    break;
                }
            case DTL_TYPE_NATIVE_REAL:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID] );
                    break;
                }
            case DTL_TYPE_NATIVE_DOUBLE:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID] );
                    break;
                }
            case DTL_TYPE_FLOAT:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s( %ld )",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID],
                                 sNumPrec );
                    break;
                }
            case DTL_TYPE_NUMBER:
                {
                    if ( sNumScale == DTL_SCALE_NA )
                    {
                        STL_DASSERT( sNumPrec == 38 );
                        
                        stlSnprintf( aStringDDL,
                                     gZtqDdlSize,
                                     "%s %s",
                                     aStringDDL,
                                     dtlDataTypeName[sTypeID] );
                    }
                    else
                    {
                        stlSnprintf( aStringDDL,
                                     gZtqDdlSize,
                                     "%s %s( %ld, %ld )",
                                     aStringDDL,
                                     dtlDataTypeName[sTypeID],
                                     sNumPrec,
                                     sNumScale );
                    }
                    break;
                }
            case DTL_TYPE_DECIMAL:
                {
                    STL_DASSERT(0);
                    STL_THROW( RAMP_ERR_INVALID_OBJECT_IDENTIFIER );
                    break;
                }
            case DTL_TYPE_CHAR:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s( %ld %s )",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID],
                                 sStringMaxLen,
                                 gDtlLengthUnitString[sLengthUnit] );
                    break;
                }
            case DTL_TYPE_VARCHAR:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s( %ld %s )",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID],
                                 sStringMaxLen,
                                 gDtlLengthUnitString[sLengthUnit] );
                    break;
                }
            case DTL_TYPE_LONGVARCHAR:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID] );
                    break;
                }
            case DTL_TYPE_CLOB:
                {
                    STL_DASSERT(0);
                    STL_THROW( RAMP_ERR_INVALID_OBJECT_IDENTIFIER );
                    break;
                }
            case DTL_TYPE_BINARY:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s( %ld )",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID],
                                 sStringMaxLen );
                    break;
                }
            case DTL_TYPE_VARBINARY:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s( %ld )",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID],
                                 sStringMaxLen );
                    break;
                }
            case DTL_TYPE_LONGVARBINARY:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID] );
                    break;
                }
            case DTL_TYPE_BLOB:
                {
                    STL_DASSERT(0);
                    STL_THROW( RAMP_ERR_INVALID_OBJECT_IDENTIFIER );
                    break;
                }
            case DTL_TYPE_DATE:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID] );
                    break;
                }
            case DTL_TYPE_TIME:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s TIME( %ld ) WITHOUT TIME ZONE",
                                 aStringDDL,
                                 sTimePrec );
                    break;
                }
            case DTL_TYPE_TIMESTAMP:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s TIMESTAMP( %ld ) WITHOUT TIME ZONE",
                                 aStringDDL,
                                 sTimePrec );
                    break;
                }
            case DTL_TYPE_TIME_WITH_TIME_ZONE:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s TIME( %ld ) WITH TIME ZONE",
                                 aStringDDL,
                                 sTimePrec );
                    break;
                }
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s TIMESTAMP( %ld ) WITH TIME ZONE",
                                 aStringDDL,
                                 sTimePrec );
                    break;
                }
            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
                {
                    switch( sIntervalType )
                    {
                        case DTL_INTERVAL_INDICATOR_YEAR:
                            {
                                stlSnprintf( aStringDDL,
                                             gZtqDdlSize,
                                             "%s INTERVAL YEAR( %ld )",
                                             aStringDDL,
                                             sIntervalPrec );
                                break;
                            }
                        case DTL_INTERVAL_INDICATOR_MONTH:
                            {
                                stlSnprintf( aStringDDL,
                                             gZtqDdlSize,
                                             "%s INTERVAL MONTH( %ld )",
                                             aStringDDL,
                                             sIntervalPrec );
                                break;
                            }
                        case DTL_INTERVAL_INDICATOR_DAY:
                            {
                                stlSnprintf( aStringDDL,
                                             gZtqDdlSize,
                                             "%s INTERVAL DAY( %ld )",
                                             aStringDDL,
                                             sIntervalPrec );
                                break;
                            }
                        case DTL_INTERVAL_INDICATOR_HOUR:
                            {
                                stlSnprintf( aStringDDL,
                                             gZtqDdlSize,
                                             "%s INTERVAL HOUR( %ld )",
                                             aStringDDL,
                                             sIntervalPrec );
                                break;
                            }
                        case DTL_INTERVAL_INDICATOR_MINUTE:
                            {
                                stlSnprintf( aStringDDL,
                                             gZtqDdlSize,
                                             "%s INTERVAL MINUTE( %ld )",
                                             aStringDDL,
                                             sIntervalPrec );
                                break;
                            }
                        case DTL_INTERVAL_INDICATOR_SECOND:
                            {
                                stlSnprintf( aStringDDL,
                                             gZtqDdlSize,
                                             "%s INTERVAL SECOND( %ld, %ld )",
                                             aStringDDL,
                                             sIntervalPrec,
                                             sTimePrec );
                                break;
                            }
                        case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                            {
                                stlSnprintf( aStringDDL,
                                             gZtqDdlSize,
                                             "%s INTERVAL YEAR( %ld ) TO MONTH",
                                             aStringDDL,
                                             sIntervalPrec );
                                break;
                            }
                        case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
                            {
                                stlSnprintf( aStringDDL,
                                             gZtqDdlSize,
                                             "%s INTERVAL DAY( %ld ) TO HOUR",
                                             aStringDDL,
                                             sIntervalPrec );
                                break;
                            }
                        case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
                            {
                                stlSnprintf( aStringDDL,
                                             gZtqDdlSize,
                                             "%s INTERVAL DAY( %ld ) TO MINUTE",
                                             aStringDDL,
                                             sIntervalPrec );
                                break;
                            }
                        case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
                            {
                                stlSnprintf( aStringDDL,
                                             gZtqDdlSize,
                                             "%s INTERVAL DAY( %ld ) TO SECOND( %ld )",
                                             aStringDDL,
                                             sIntervalPrec,
                                             sTimePrec );
                                break;
                            }
                        case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
                            {
                                stlSnprintf( aStringDDL,
                                             gZtqDdlSize,
                                             "%s INTERVAL HOUR( %ld ) TO MINUTE",
                                             aStringDDL,
                                             sIntervalPrec );
                                break;
                            }
                        case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
                            {
                                stlSnprintf( aStringDDL,
                                             gZtqDdlSize,
                                             "%s INTERVAL HOUR( %ld ) TO SECOND( %ld )",
                                             aStringDDL,
                                             sIntervalPrec,
                                             sTimePrec );
                                break;
                            }
                        case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
                            {
                                stlSnprintf( aStringDDL,
                                             gZtqDdlSize,
                                             "%s INTERVAL MINUTE( %ld ) TO SECOND( %ld )",
                                             aStringDDL,
                                             sIntervalPrec,
                                             sTimePrec );
                                break;
                            }
                        default:
                            {
                                STL_DASSERT(0);
                                STL_THROW( RAMP_ERR_INVALID_OBJECT_IDENTIFIER );
                                break;
                            }
                    }
                    
                    break;
                }
            case DTL_TYPE_ROWID:
                {
                    stlSnprintf( aStringDDL,
                                 gZtqDdlSize,
                                 "%s %s",
                                 aStringDDL,
                                 dtlDataTypeName[sTypeID] );
                    break;
                }
            default:
                {
                    STL_DASSERT(0);
                    STL_THROW( RAMP_ERR_INVALID_OBJECT_IDENTIFIER );
                    break;
                }
        }
        
        sColCount++;

        /**
         * Column 의 DEFAULT 값 출력 
         */
        
        if ( sDefaultInd == SQL_NULL_DATA )
        {
            /**
             * DEFAULT 가 없는 경우
             */
        }
        else
        {
            /**
             * DEFAULT 가 있는 경우, 'DEFAULT 1' 과 같이 구문이 저장되어 있다.
             */
            
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n            %s ",
                         aStringDDL,
                         sDefaultLong );
        }

        /**
         * Identity Column 정보 출력
         */

        if ( stlStrcmp( sIsIdentity, "YES" ) == 0 )
        {
            /**
             * Identity Column 인 경우
             */

            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n            GENERATED %s AS IDENTITY "
                         "\n            ( "
                         "\n                START WITH %ld "
                         "\n                INCREMENT BY %ld "
                         "\n                MAXVALUE %ld "
                         "\n                MINVALUE %ld ",
                         aStringDDL,
                         sIdentityGen,
                         sIdentityStart,
                         sIdentityInc,
                         sIdentityMax,
                         sIdentityMin );

            if ( sIsCycle[0] == 'Y' )
            {
                stlSnprintf( aStringDDL,
                             gZtqDdlSize,
                             "%s"
                             "\n                CYCLE ",
                             aStringDDL );
            }
            else
            {
                stlSnprintf( aStringDDL,
                             gZtqDdlSize,
                             "%s"
                             "\n                NO CYCLE ",
                             aStringDDL );
            }

            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n                CACHE %ld "
                         "\n            ) ",
                         aStringDDL,
                         sIdentityCache  );
              
        }
        else
        {
            /**
             * Identity Column 이 아닌 경우
             */
        }
    }

    /**
     * column 끝
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    ) ",
                 aStringDDL );
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    /************************************************************
     * CREATE TABLE 구문의 Tail 생성
     ************************************************************/

    /**
     * WITH COLUMNAR OPTIONS
     */
    
    if ( stlStrcmp( sCacheTableType, ZTQ_COLUMNAR_TABLE_TYPE ) == 0 )
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    WITH COLUMNAR OPTIONS ",
                     aStringDDL );
    }
    else
    {
        /**
         * table physical attribute
         */
        
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    PCTFREE  %ld "
                     "\n    PCTUSED  %ld "
                     "\n    INITRANS %ld "
                     "\n    MAXTRANS %ld ",
                     aStringDDL,
                     sPctFree,
                     sPctUsed,
                     sIniTrans,
                     sMaxTrans );
    }
    
    /**
     * segment attribute
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    STORAGE "
                 "\n    ( "
                 "\n        INITIAL %ld "
                 "\n        NEXT    %ld "
                 "\n        MINSIZE %ld "
                 "\n        MAXSIZE %ld "
                 "\n    ) ",
                 aStringDDL,
                 sSegInit,
                 sSegNext,
                 sSegMin,
                 sSegMax );
    
    /**
     * TABLESPACE tablespace 
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    TABLESPACE \"%s\" ",
                 aStringDDL,
                 sSpaceName );
    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        STL_TRY( stlRestoreRegionMem( aAllocator,
                                      &sMark,
                                      STL_FALSE, /* aFree */
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_NOT_ENOUGH_DDLSIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_ENOUGH_DDLSIZE,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        (void) stlRestoreRegionMem( aAllocator,
                                    &sMark,
                                    STL_FALSE, /* aFree */
                                    aErrorStack );
    }
    
    return STL_FAILURE;
}




stlStatus ztqPrintTableConstraint( stlInt64        aTableID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sConstID;
    SQLLEN      sConstInd;
    
    /************************************************************
     * CONSTRAINT 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT CONSTRAINT_ID "
                 "  FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS "
                 " WHERE CONSTRAINT_TYPE IN ( 'NOT NULL', 'PRIMARY KEY', 'UNIQUE' ) "
                 "   AND TABLE_ID = %ld "
                 " ORDER BY  "
                 "       CASE WHEN CONSTRAINT_TYPE = 'NOT NULL'    THEN 1 "
                 "            WHEN CONSTRAINT_TYPE = 'PRIMARY KEY' THEN 2 "
                 "            WHEN CONSTRAINT_TYPE = 'UNIQUE'      THEN 3 "
                 "            ELSE NULL "
                 "            END "
                 "     , CONSTRAINT_ID ",
                 aTableID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sConstID,
                            STL_SIZEOF(sConstID),
                            & sConstInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * Constraint ID 에 해당하는 ALTER TABLE 구문 출력
         */
        
        STL_TRY( ztqPrintAddConstraint( sConstID,
                                        aStringDDL,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}







stlStatus ztqPrintTableCreateIndex( stlInt64        aTableID,
                                    stlChar       * aStringDDL,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sIndexID;
    SQLLEN      sIndexInd;
    
    /************************************************************
     * INDEX 목록 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT idx.INDEX_ID "
                 "  FROM DEFINITION_SCHEMA.INDEXES idx"
                 "     , DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE ikey"
                 " WHERE idx.INDEX_ID = ikey.INDEX_ID "
                 "   AND idx.BY_CONSTRAINT = FALSE "
                 "   AND ikey.TABLE_ID = %ld "
                 " ORDER BY INDEX_ID ",
                 aTableID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sIndexID,
                            STL_SIZEOF(sIndexID),
                            & sIndexInd,
                            aErrorStack )
             == STL_SUCCESS );

    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        /**
         * IndexID 에 해당하는 CREATE INDEX 구문 출력
         */

        STL_TRY( ztqPrintCreateIndex( sIndexID,
                                      aStringDDL,
                                      aAllocator,
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintTableIdentity( stlInt64        aTableID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sTableInd;
    stlChar     sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sColumnInd;

    stlInt64    sRestartValue;
    SQLLEN      sRestartInd;
    
    /************************************************************
     * IDENTITY 컬럼 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME   "
                 "      , sch.SCHEMA_NAME           "
                 "      , tab.TABLE_NAME            "
                 "      , col.COLUMN_NAME           "
                 "      , xseq.RESTART_VALUE        "
                 "   FROM "
                 "        DEFINITION_SCHEMA.TABLES           AS tab  "
                 "      , DEFINITION_SCHEMA.COLUMNS          AS col  "
                 "      , FIXED_TABLE_SCHEMA.X$SEQUENCE      AS xseq "
                 "      , DEFINITION_SCHEMA.SCHEMATA         AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS   AS auth  "
                 "  WHERE "
                 "        col.TABLE_ID             = tab.TABLE_ID "
                 "    AND col.IDENTITY_PHYSICAL_ID = xseq.PHYSICAL_ID "
                 "    AND col.SCHEMA_ID            = sch.SCHEMA_ID "
                 "    AND col.OWNER_ID             = auth.AUTH_ID "
                 "    AND col.IS_IDENTITY          = TRUE "
                 "    AND tab.TABLE_ID             = %ld ",
                 aTableID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sTableName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sColumnName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sColumnInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_SBIGINT,
                            & sRestartValue,
                            STL_SIZEOF(sRestartValue),
                            & sRestartInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    if ( sResult == SQL_SUCCESS )
    {
        /* Identity Column 이 존재함 */
    }
    else
    {
        /* Identity Column 이 없음 */
        STL_THROW( RAMP_FINISH );
    }

    /************************************************************
     * ALTER TABLE 구문 생성
     ************************************************************/

    /**
     * SET SESSION AUTHORIZATION grantor
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"%s\"; ",
                 sOwnerName );
    
    /**
     * ALTER TABLE
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nALTER TABLE \"%s\".\"%s\" ",
                 aStringDDL,
                 sSchemaName,
                 sTableName );

    /**
     * ALTER COLUMN
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    ALTER COLUMN \"%s\" "
                 "\n    RESTART WITH %ld ",
                 aStringDDL,
                 sColumnName,
                 sRestartValue );
    
    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintTableSupplemental( stlInt64        aTableID,
                                     stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sTableInd;
    
    /************************************************************
     * SUPPLEMENTAL 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME   "
                 "      , sch.SCHEMA_NAME           "
                 "      , tab.TABLE_NAME            "
                 "   FROM "
                 "        DEFINITION_SCHEMA.TABLES           AS tab  "
                 "      , DEFINITION_SCHEMA.SCHEMATA         AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS   AS auth  "
                 "  WHERE "
                 "        tab.SCHEMA_ID            = sch.SCHEMA_ID "
                 "    AND tab.OWNER_ID             = auth.AUTH_ID "
                 "    AND tab.IS_SET_SUPPLOG_PK    = TRUE "
                 "    AND tab.TABLE_ID             = %ld ",
                 aTableID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sTableName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );


    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    if ( sResult == SQL_SUCCESS )
    {
        /* Supplemental Logging 이 존재함 */
    }
    else
    {
        /* Supplemental Logging 이 없음 */
        STL_THROW( RAMP_FINISH );
    }

    /************************************************************
     * ALTER TABLE 구문 생성
     ************************************************************/

    /**
     * SET SESSION AUTHORIZATION grantor
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"%s\"; ",
                 sOwnerName );
    
    /**
     * ALTER TABLE
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nALTER TABLE \"%s\".\"%s\" ",
                 aStringDDL,
                 sSchemaName,
                 sTableName );

    /**
     * ADD SUPPLEMENTAL LOG
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    ADD SUPPLEMENTAL LOG DATA ( PRIMARY KEY ) COLUMNS ",
                 aStringDDL );
    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}





stlStatus ztqPrintGrantRelation( stlInt64        aRelationID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlRegionMark   sMark;
    stlBool         sIsMarked = STL_FALSE;

    ztqPrivOrder  sPrivOrder;

    stlInt64    sOrgColumnID;
    stlChar     sOrgColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    stlInt64    sOrgPrivType;
    stlChar     sOrgPrivName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    stlInt64    sOwnerID;
    SQLLEN      sOwnerInd;

    stlInt64    sGrantorID;
    SQLLEN      sGrantorInd;
    stlChar     sGrantorName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGrantorNameInd;
    
    stlInt64    sGranteeID;
    SQLLEN      sGranteeInd;
    stlChar     sGranteeName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGranteeNameInd;

    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sRelName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sRelInd;
    stlInt64    sColumnID;
    SQLLEN      sColumnInd;
    stlChar     sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sColumnNameInd;
    
    stlInt64    sPrivType;
    SQLLEN      sPrivTypeInd;
    stlChar     sPrivName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sPrivNameInd;
    
    stlChar     sGrantable[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGrantInd;

    stlChar     sIsBuiltIn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlStrncpy( sIsBuiltIn, "NO", STL_MAX_SQL_IDENTIFIER_LENGTH );
    

    /**
     * Memory Mark 설정
     */
    
    STL_TRY( stlMarkRegionMem( aAllocator,
                               &sMark,
                               aErrorStack ) == STL_SUCCESS );
    sIsMarked = STL_TRUE;
    
    /************************************************************
     * GRANT priv ON TABLE 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       tab.OWNER_ID "
                 "     , grantor.AUTH_ID "
                 "     , grantor.AUTHORIZATION_NAME "
                 "     , grantee.AUTH_ID "
                 "     , grantee.AUTHORIZATION_NAME "
                 "     , sch.SCHEMA_NAME "
                 "     , tab.TABLE_NAME "
                 "     , priv.PRIVILEGE_TYPE_ID "
                 "     , priv.PRIVILEGE_TYPE "
                 "     , CAST( CASE WHEN priv.IS_GRANTABLE = TRUE THEN 'YES' "
                 "                                                ELSE 'NO' "
                 "             END AS VARCHAR(3 OCTETS) ) "
                 "  FROM "
                 "       DEFINITION_SCHEMA.TABLES            AS tab "
                 "     , DEFINITION_SCHEMA.TABLE_PRIVILEGES  AS priv "
                 "     , DEFINITION_SCHEMA.SCHEMATA          AS sch "
                 "     , DEFINITION_SCHEMA.AUTHORIZATIONS    AS grantor "
                 "     , DEFINITION_SCHEMA.AUTHORIZATIONS    AS grantee "
                 " WHERE "
                 "       grantor.AUTHORIZATION_NAME <> '_SYSTEM' "
                 "   AND grantee.AUTH_ID >= ( SELECT AUTH_ID "
                 "                              FROM DEFINITION_SCHEMA.AUTHORIZATIONS "
                 "                             WHERE AUTHORIZATION_NAME = 'PUBLIC' ) "
                 "   AND tab.TABLE_ID     = priv.TABLE_ID "
                 "   AND tab.SCHEMA_ID    = sch.SCHEMA_ID "
                 "   AND priv.GRANTOR_ID  = grantor.AUTH_ID "
                 "   AND priv.GRANTEE_ID  = grantee.AUTH_ID "
                 "   AND tab.TABLE_ID = %ld "
                 " ORDER BY "
                 "       priv.PRIVILEGE_TYPE_ID "
                 "     , grantor.AUTH_ID "
                 "     , grantee.AUTH_ID ",
                 aRelationID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sOwnerID,
                            STL_SIZEOF(sOwnerID),
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_SBIGINT,
                            & sGrantorID,
                            STL_SIZEOF(sGrantorID),
                            & sGrantorInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sGrantorName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGrantorNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_SBIGINT,
                            & sGranteeID,
                            STL_SIZEOF(sGranteeID),
                            & sGranteeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_CHAR,
                            sGranteeName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGranteeNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            6,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            7,
                            SQL_C_CHAR,
                            sRelName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sRelInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            8,
                            SQL_C_SBIGINT,
                            & sPrivType,
                            STL_SIZEOF(sPrivType),
                            & sPrivTypeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            9,
                            SQL_C_CHAR,
                            sPrivName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sPrivNameInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            10,
                            SQL_C_CHAR,
                            sGrantable,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGrantInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    /**
     * 최초 Fetch
     */
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_TABLE_FINISH );

    /**
     * Privilege Print Order 결정을 위한 초기화
     */
    
    sOrgPrivType = sPrivType;
    stlStrncpy( sOrgPrivName, sPrivName, STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    ztqInitPrivOrder( & sPrivOrder );

    STL_TRY( ztqAddPrivItem( & sPrivOrder,
                             sGrantorID,
                             sGrantorName,
                             sGranteeID,
                             sGranteeName,
                             sIsBuiltIn, 
                             sGrantable,
                             aAllocator,
                             aErrorStack )
             == STL_SUCCESS );
    
    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            ztqBuildPrivOrder( & sPrivOrder, sOwnerID );
            
            STL_TRY( ztqPrintGrantByPrivOrder( aStringDDL,
                                               & sPrivOrder,
                                               ZTQ_PRIV_OBJECT_TABLE,
                                               sOrgPrivName,
                                               sSchemaName, 
                                               sRelName,
                                               NULL, /* aColumnName */
                                               aErrorStack )
                     == STL_SUCCESS );
            break;
        }

        if ( sPrivType == sOrgPrivType )
        {
            /**
             * 이전과 동일한 Privilege Type 인 경우
             */

            STL_TRY( ztqAddPrivItem( & sPrivOrder,
                                     sGrantorID,
                                     sGrantorName,
                                     sGranteeID,
                                     sGranteeName,
                                     sIsBuiltIn,
                                     sGrantable,
                                     aAllocator,
                                     aErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * 서로 다른 Privilege Type 인 경우
             */

            ztqBuildPrivOrder( & sPrivOrder, sOwnerID );
            
            STL_TRY( ztqPrintGrantByPrivOrder( aStringDDL,
                                               & sPrivOrder,
                                               ZTQ_PRIV_OBJECT_TABLE,
                                               sOrgPrivName,
                                               sSchemaName, 
                                               sRelName,
                                               NULL, /* aColumnName */
                                               aErrorStack )
                     == STL_SUCCESS );

            /**
             * 메모리 재사용
             */
            
            sIsMarked = STL_FALSE;
            STL_TRY( stlRestoreRegionMem( aAllocator,
                                          &sMark,
                                          STL_FALSE, /* aFree */
                                          aErrorStack ) == STL_SUCCESS );
            STL_TRY( stlMarkRegionMem( aAllocator,
                                       &sMark,
                                       aErrorStack ) == STL_SUCCESS );
            sIsMarked = STL_TRUE;

            /**
             * Privilege Print Order 결정을 위한 초기화
             */
            
            sOrgPrivType = sPrivType;
            stlStrncpy( sOrgPrivName, sPrivName, STL_MAX_SQL_IDENTIFIER_LENGTH );
            
            ztqInitPrivOrder( & sPrivOrder );
            
            STL_TRY( ztqAddPrivItem( & sPrivOrder,
                                     sGrantorID,
                                     sGrantorName,
                                     sGranteeID,
                                     sGranteeName,
                                     sIsBuiltIn,
                                     sGrantable,
                                     aAllocator,
                                     aErrorStack )
                     == STL_SUCCESS );
        }
    }

    /******************************
     * 마무리 
     ******************************/

    STL_RAMP( RAMP_TABLE_FINISH );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /**
     * 메모리 재사용
     */
            
    sIsMarked = STL_FALSE;
    STL_TRY( stlRestoreRegionMem( aAllocator,
                                  &sMark,
                                  STL_FALSE, /* aFree */
                                  aErrorStack ) == STL_SUCCESS );
    STL_TRY( stlMarkRegionMem( aAllocator,
                               &sMark,
                               aErrorStack ) == STL_SUCCESS );
    sIsMarked = STL_TRUE;

    
    /************************************************************
     * GRANT column priv ON TABLE 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "       tab.OWNER_ID "
                 "     , grantor.AUTH_ID "
                 "     , grantor.AUTHORIZATION_NAME "
                 "     , grantee.AUTH_ID "
                 "     , grantee.AUTHORIZATION_NAME "
                 "     , sch.SCHEMA_NAME "
                 "     , tab.TABLE_NAME "
                 "     , col.COLUMN_ID "
                 "     , col.COLUMN_NAME "
                 "     , cpriv.PRIVILEGE_TYPE_ID "
                 "     , cpriv.PRIVILEGE_TYPE "
                 "     , CAST( CASE WHEN cpriv.IS_GRANTABLE = TRUE THEN 'YES' "
                 "                                                 ELSE 'NO' "
                 "             END AS VARCHAR(3 OCTETS) ) "
                 "  FROM "
                 "       DEFINITION_SCHEMA.TABLES             AS tab "
                 "     , DEFINITION_SCHEMA.COLUMNS            AS col "
                 "     , DEFINITION_SCHEMA.COLUMN_PRIVILEGES  AS cpriv "
                 "     , DEFINITION_SCHEMA.SCHEMATA           AS sch "
                 "     , DEFINITION_SCHEMA.AUTHORIZATIONS     AS grantor "
                 "     , DEFINITION_SCHEMA.AUTHORIZATIONS     AS grantee "
                 " WHERE "
                 "       grantor.AUTHORIZATION_NAME <> '_SYSTEM' "
                 "   AND grantee.AUTH_ID >= ( SELECT AUTH_ID "
                 "                              FROM DEFINITION_SCHEMA.AUTHORIZATIONS "
                 "                             WHERE AUTHORIZATION_NAME = 'PUBLIC' ) "
                 "   AND tab.TABLE_ID      = col.TABLE_ID "
                 "   AND col.COLUMN_ID     = cpriv.COLUMN_ID "
                 "   AND tab.SCHEMA_ID     = sch.SCHEMA_ID "
                 "   AND cpriv.GRANTOR_ID  = grantor.AUTH_ID "
                 "   AND cpriv.GRANTEE_ID  = grantee.AUTH_ID "
                 "   AND ( cpriv.PRIVILEGE_TYPE, cpriv.IS_GRANTABLE ) NOT IN "
                 "       ( "
                 "         SELECT tpriv.PRIVILEGE_TYPE, tpriv.IS_GRANTABLE "
                 "           FROM "
                 "                DEFINITION_SCHEMA.TABLES  AS subtab "
                 "              , DEFINITION_SCHEMA.TABLE_PRIVILEGES  AS tpriv "
                 "              , DEFINITION_SCHEMA.AUTHORIZATIONS    AS subgrantor "
                 "          WHERE "
                 "                subgrantor.AUTHORIZATION_NAME <> '_SYSTEM' "
                 "            AND subtab.TABLE_ID   = tab.TABLE_ID "
                 "            AND subtab.TABLE_ID   = tpriv.TABLE_ID "
                 "            AND tpriv.GRANTOR_ID  = subgrantor.AUTH_ID "
                 "       ) "
                 "   AND tab.TABLE_ID = %ld "
                 " ORDER BY "
                 "       col.COLUMN_ID "
                 "     , cpriv.PRIVILEGE_TYPE_ID "
                 "     , grantor.AUTH_ID "
                 "     , grantee.AUTH_ID ",
                 aRelationID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sOwnerID,
                            STL_SIZEOF(sOwnerID),
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_SBIGINT,
                            & sGrantorID,
                            STL_SIZEOF(sGrantorID),
                            & sGrantorInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sGrantorName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGrantorNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_SBIGINT,
                            & sGranteeID,
                            STL_SIZEOF(sGranteeID),
                            & sGranteeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_CHAR,
                            sGranteeName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGranteeNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            6,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            7,
                            SQL_C_CHAR,
                            sRelName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sRelInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            8,
                            SQL_C_SBIGINT,
                            & sColumnID,
                            STL_SIZEOF(sColumnID),
                            & sColumnInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            9,
                            SQL_C_CHAR,
                            sColumnName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sColumnNameInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            10,
                            SQL_C_SBIGINT,
                            & sPrivType,
                            STL_SIZEOF(sPrivType),
                            & sPrivTypeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            11,
                            SQL_C_CHAR,
                            sPrivName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sPrivNameInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            12,
                            SQL_C_CHAR,
                            sGrantable,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGrantInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    /**
     * 최초 Fetch
     */
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_COLUMN_FINISH );

    /**
     * Privilege Print Order 결정을 위한 초기화
     */

    sOrgColumnID = sColumnID;
    stlStrncpy( sOrgColumnName, sColumnName, STL_MAX_SQL_IDENTIFIER_LENGTH );
    sOrgPrivType = sPrivType;
    stlStrncpy( sOrgPrivName, sPrivName, STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    ztqInitPrivOrder( & sPrivOrder );

    STL_TRY( ztqAddPrivItem( & sPrivOrder,
                             sGrantorID,
                             sGrantorName,
                             sGranteeID,
                             sGranteeName,
                             sIsBuiltIn, 
                             sGrantable,
                             aAllocator,
                             aErrorStack )
             == STL_SUCCESS );
    
    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            ztqBuildPrivOrder( & sPrivOrder, sOwnerID );
            
            STL_TRY( ztqPrintGrantByPrivOrder( aStringDDL,
                                               & sPrivOrder,
                                               ZTQ_PRIV_OBJECT_COLUMN,
                                               sOrgPrivName,
                                               sSchemaName, 
                                               sRelName,
                                               sOrgColumnName, 
                                               aErrorStack )
                     == STL_SUCCESS );
            break;
        }

        if ( (sColumnID == sOrgColumnID) && (sPrivType == sOrgPrivType) )
        {
            /**
             * 이전과 동일한 Column Privilege Type 인 경우
             */

            STL_TRY( ztqAddPrivItem( & sPrivOrder,
                                     sGrantorID,
                                     sGrantorName,
                                     sGranteeID,
                                     sGranteeName,
                                     sIsBuiltIn,
                                     sGrantable,
                                     aAllocator,
                                     aErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * 서로 다른 Privilege Type 인 경우
             */

            ztqBuildPrivOrder( & sPrivOrder, sOwnerID );
            
            STL_TRY( ztqPrintGrantByPrivOrder( aStringDDL,
                                               & sPrivOrder,
                                               ZTQ_PRIV_OBJECT_COLUMN,
                                               sOrgPrivName,
                                               sSchemaName, 
                                               sRelName,
                                               sOrgColumnName, 
                                               aErrorStack )
                     == STL_SUCCESS );

            /**
             * 메모리 재사용
             */
            
            sIsMarked = STL_FALSE;
            STL_TRY( stlRestoreRegionMem( aAllocator,
                                          &sMark,
                                          STL_FALSE, /* aFree */
                                          aErrorStack ) == STL_SUCCESS );
            STL_TRY( stlMarkRegionMem( aAllocator,
                                       &sMark,
                                       aErrorStack ) == STL_SUCCESS );
            sIsMarked = STL_TRUE;

            /**
             * Privilege Print Order 결정을 위한 초기화
             */
            
            sOrgColumnID = sColumnID;
            stlStrncpy( sOrgColumnName, sColumnName, STL_MAX_SQL_IDENTIFIER_LENGTH );
            sOrgPrivType = sPrivType;
            stlStrncpy( sOrgPrivName, sPrivName, STL_MAX_SQL_IDENTIFIER_LENGTH );
            
            ztqInitPrivOrder( & sPrivOrder );
            
            STL_TRY( ztqAddPrivItem( & sPrivOrder,
                                     sGrantorID,
                                     sGrantorName,
                                     sGranteeID,
                                     sGranteeName,
                                     sIsBuiltIn,
                                     sGrantable,
                                     aAllocator,
                                     aErrorStack )
                     == STL_SUCCESS );
        }
    }

    /*******************************
     * 마무리 
     *******************************/

    STL_RAMP( RAMP_COLUMN_FINISH );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /**
     * Memory Mark 해제
     */
    
    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        STL_TRY( stlRestoreRegionMem( aAllocator,
                                      &sMark,
                                      STL_FALSE, /* aFree */
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        (void) stlRestoreRegionMem( aAllocator,
                                    &sMark,
                                    STL_FALSE, /* aFree */
                                    aErrorStack );
    }
    
    return STL_FAILURE;
}




stlStatus ztqPrintCommentRelation( stlInt64        aRelationID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sRelName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sRelInd;
    stlChar     sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sColumnInd;
    stlChar     sComment[ZTQ_MAX_COMMENT_LENGTH];
    SQLLEN      sCommentInd;

    /************************************************************
     * COMMENT 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME  "
                 "      , sch.SCHEMA_NAME          "
                 "      , tab.TABLE_NAME           "
                 "      , tab.COMMENTS             "
                 "   FROM "
                 "        DEFINITION_SCHEMA.TABLES                AS tab  "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS        AS auth  "
                 "  WHERE "
                 "        tab.SCHEMA_ID   = sch.SCHEMA_ID "
                 "    AND tab.OWNER_ID    = auth.AUTH_ID "
                 "    AND tab.TABLE_ID    = %ld ",
                 aRelationID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sRelName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sRelInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sComment,
                            ZTQ_MAX_COMMENT_LENGTH,
                            & sCommentInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * COMMENT ON TABLE 구문 생성
     ************************************************************/

    if ( sCommentInd == SQL_NULL_DATA )
    {
        /* comment 가 없는 경우 */
    }
    else
    {
        /**
         * SET SESSION AUTHORIZATION owner
         */
    
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "\nSET SESSION AUTHORIZATION \"%s\"; ",
                     sOwnerName );
    
        /**
         * COMMENT
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\nCOMMENT "
                     "\n    ON TABLE \"%s\".\"%s\" ",
                     aStringDDL,
                     sSchemaName,
                     sRelName );

        /**
         * IS 'comment'
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    IS '%s' ",
                     aStringDDL,
                     sComment );
    
        STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
    }

    /************************************************************
     * COMMENT ON COLUMN 구문 생성
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        col.COLUMN_NAME          "
                 "      , col.COMMENTS             "
                 "   FROM "
                 "        DEFINITION_SCHEMA.TABLES                AS tab  "
                 "      , DEFINITION_SCHEMA.COLUMNS               AS col  "
                 "  WHERE "
                 "        tab.TABLE_ID = col.TABLE_ID "
                 "    AND tab.TABLE_ID = %ld "
                 "  ORDER BY "
                 "        col.PHYSICAL_ORDINAL_POSITION ",
                 aRelationID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sColumnName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sColumnInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sComment,
                            ZTQ_MAX_COMMENT_LENGTH,
                            & sCommentInd,
                            aErrorStack )
             == STL_SUCCESS );

    while (1)
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }

        if ( sCommentInd == SQL_NULL_DATA )
        {
            /* comment 가 없는 경우 */
        }
        else
        {
            /**
             * SET SESSION AUTHORIZATION owner
             */
            
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "\nSET SESSION AUTHORIZATION \"%s\"; ",
                         sOwnerName );
            
            /**
             * COMMENT
             */
            
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\nCOMMENT "
                         "\n    ON COLUMN \"%s\".\"%s\".\"%s\" ",
                         aStringDDL,
                         sSchemaName,
                         sRelName,
                         sColumnName );

            /**
             * IS 'comment'
             */
            
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n    IS '%s' ",
                         aStringDDL,
                         sComment );
            
            STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
        }
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}





/****************************************************************************
 * Constraint
 ****************************************************************************/

stlStatus ztqGetConstraintID( ztqIdentifier * aIdentifierList,
                              stlAllocator  * aAllocator,
                              stlInt64      * aConstID,
                              stlErrorStack * aErrorStack )
{
    stlChar  * sIdentifier = NULL;
    stlInt32   sLen = 0;

    stlBool     sIsQualified = STL_FALSE;
    stlChar     sConstName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    /************************************************************
     * double-quoted identifier 처리 
     ************************************************************/

    if ( aIdentifierList->mNext == NULL )
    {
        /**
         * constraint_name 만 기술한 경우 
         */

        sIsQualified = STL_FALSE;
        sIdentifier = aIdentifierList->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sConstName, sIdentifier + 1, sLen - 2 );
            sConstName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sConstName, sIdentifier );
        }
    }
    else
    {
        /**
         * schema_name.constraint_name 으로  기술한 경우 
         */
        
        sIsQualified = STL_TRUE;

        sIdentifier = aIdentifierList->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sSchemaName, sIdentifier + 1, sLen - 2 );
            sSchemaName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sSchemaName, sIdentifier );
        }

        sIdentifier = ((ztqIdentifier *) aIdentifierList->mNext)->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sConstName, sIdentifier + 1, sLen - 2 );
            sConstName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sConstName, sIdentifier );
        }
    }

    /************************************************************
     * Constraint ID 획득
     ************************************************************/

    if ( sIsQualified == STL_TRUE )
    {
        STL_TRY( ztqGetQualifiedConstraintID( sSchemaName,
                                              sConstName,
                                              aAllocator,
                                              aConstID,
                                              aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztqGetNonQualifiedConstraintID( sConstName,
                                                 aAllocator,
                                                 aConstID,
                                                 aErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqGetQualifiedConstraintID( stlChar       * aSchemaName,
                                       stlChar       * aConstName,
                                       stlAllocator  * aAllocator,
                                       stlInt64      * aConstID,
                                       stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sConstID;
    SQLLEN      sConstInd;
    
    /************************************************************
     * CONSTRAINT ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT const.CONSTRAINT_ID "
                 "  FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS const "
                 "     , DEFINITION_SCHEMA.SCHEMATA sch "
                 " WHERE const.CONSTRAINT_SCHEMA_ID = sch.SCHEMA_ID "
                 "   AND const.CONSTRAINT_NAME = '%s' "
                 "   AND sch.SCHEMA_NAME = '%s' ",
                 aConstName,
                 aSchemaName );

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sConstID,
                            STL_SIZEOF(sConstID),
                            & sConstInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aConstID = sConstID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqGetNonQualifiedConstraintID( stlChar       * aConstName,
                                          stlAllocator  * aAllocator,
                                          stlInt64      * aConstID,
                                          stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sConstID;
    SQLLEN      sConstInd;
    
    /************************************************************
     * CONSTRAINT ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        const.CONSTRAINT_ID "
                 "   FROM "
                 "        DEFINITION_SCHEMA.SCHEMATA           AS sch "
                 "      , DEFINITION_SCHEMA.TABLE_CONSTRAINTS  AS const "
                 "      , ( "
                 "         ( SELECT "
                 "                sch1.SCHEMA_ID  "
                 "              , path1.SEARCH_ORDER  "
                 "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path1 "
                 "              , DEFINITION_SCHEMA.SCHEMATA         AS sch1 "
                 "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr1 "
                 "          WHERE "
                 "                usr1.AUTHORIZATION_NAME = CURRENT_USER "
                 "            AND path1.SCHEMA_ID         = sch1.SCHEMA_ID "
                 "            AND path1.AUTH_ID           = usr1.AUTH_ID "
                 "          ORDER BY "
                 "                path1.SEARCH_ORDER "
                 "         ) "
                 "       UNION ALL "
                 "         ( SELECT "
                 "                sch2.SCHEMA_ID "
                 "              , ( SELECT COUNT(*) "
                 "                    FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH "
                 "                   WHERE AUTH_ID = USER_ID() ) "
                 "                + path2.SEARCH_ORDER "
                 "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path2 "
                 "              , DEFINITION_SCHEMA.SCHEMATA         AS sch2  "
                 "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr2 "
                 "          WHERE "
                 "                usr2.AUTHORIZATION_NAME = 'PUBLIC' "
                 "            AND path2.SCHEMA_ID         = sch2.SCHEMA_ID "
                 "            AND path2.AUTH_ID           = usr2.AUTH_ID "
                 "          ORDER BY "
                 "                path2.SEARCH_ORDER "
                 "         ) "
                 "       ) AS upath ( SCHEMA_ID, SEARCH_ORDER ) "
                 " WHERE const.CONSTRAINT_SCHEMA_ID = sch.SCHEMA_ID "
                 "   AND sch.SCHEMA_ID = upath.SCHEMA_ID "
                 "   AND const.CONSTRAINT_NAME = '%s' "
                 " ORDER BY "
                 "       upath.SEARCH_ORDER  "
                 " FETCH 1 ",
                 aConstName );
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sConstID,
                            STL_SIZEOF(sConstID),
                            & sConstInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aConstID = sConstID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintAddConstraint( stlInt64        aConstID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlChar     sConstType[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sConstInd;
    
    /************************************************************
     * CONSTRAINT 유형 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT const.CONSTRAINT_TYPE "
                 "  FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS const "
                 " WHERE const.CONSTRAINT_ID = %ld ",
                 aConstID );

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sConstType,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sConstInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * Constraint Type 에 따른 DDL 구문 생성
     ************************************************************/

    if ( stlStrcmp( sConstType, ZTQ_CONSTRANT_NOT_NULL ) == 0 )
    {
        STL_TRY( ztqPrintNotNullConstraint( aConstID,
                                            aStringDDL,
                                            aAllocator,
                                            aErrorStack )
                 == STL_SUCCESS );
    }
    else if ( stlStrcmp( sConstType, ZTQ_CONSTRANT_PRIMARY_KEY ) == 0 )
    {
        STL_TRY( ztqPrintUniquePrimaryConstraint( aConstID,
                                                  aStringDDL,
                                                  aAllocator,
                                                  aErrorStack )
                 == STL_SUCCESS );
    }
    else if ( stlStrcmp( sConstType, ZTQ_CONSTRANT_UNIQUE ) == 0 )
    {
        STL_TRY( ztqPrintUniquePrimaryConstraint( aConstID,
                                                  aStringDDL,
                                                  aAllocator,
                                                  aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Foreign Key, Check Constraint 를 생성할 수 있는 DDL 구문을 지원하지 않는다.
         */
        
        STL_THROW( RAMP_ERR_INVALID_OBJECT_IDENTIFIER );
    }
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintNotNullConstraint( stlInt64        aConstID,
                                     stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlChar     sConstOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sConstOwnerInd;
    stlChar     sConstSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sConstSchemaInd;
    stlChar     sConstName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sConstInd;

    stlChar     sTableSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sTableSchemaInd;
    stlChar     sTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sTableInd;
    stlChar     sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sColumnInd;
    stlChar     sDeferrable[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sDeferrableInd;
    stlChar     sInitDeferred[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sInitDeferredInd;

    
    /************************************************************
     * ALTER TABLE 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME  "
                 "      , tsch.SCHEMA_NAME         "
                 "      , tab.TABLE_NAME           "
                 "      , csch.SCHEMA_NAME         "
                 "      , cst.CONSTRAINT_NAME      "
                 "      , col.COLUMN_NAME          "
                 "      , CAST ( CASE WHEN cst.IS_DEFERRABLE = TRUE THEN 'YES' "
                 "                                                  ELSE 'NO' "
                 "                     END AS VARCHAR(3 OCTETS) ) "
                 "      , CAST ( CASE WHEN cst.INITIALLY_DEFERRED = TRUE THEN 'YES' "
                 "                                                       ELSE 'NO' "
                 "                     END AS VARCHAR(3 OCTETS) ) "
                 "   FROM "
                 "        DEFINITION_SCHEMA.TABLE_CONSTRAINTS     AS cst "
                 "      , DEFINITION_SCHEMA.CHECK_COLUMN_USAGE    AS ccu "
                 "      , DEFINITION_SCHEMA.TABLES                AS tab  "
                 "      , DEFINITION_SCHEMA.COLUMNS               AS col  "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS csch  "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS tsch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS        AS auth  "
                 "  WHERE "
                 "        cst.CONSTRAINT_ID        = ccu.CONSTRAINT_ID "
                 "    AND cst.CONSTRAINT_SCHEMA_ID = csch.SCHEMA_ID "
                 "    AND cst.CONSTRAINT_OWNER_ID  = auth.AUTH_ID "
                 "    AND cst.TABLE_SCHEMA_ID      = tsch.SCHEMA_ID "
                 "    AND cst.TABLE_ID             = tab.TABLE_ID "
                 "    AND ccu.COLUMN_ID            = col.COLUMN_ID "
                 "    AND cst.CONSTRAINT_ID        = %ld ",
                 aConstID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sConstOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sConstOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sTableSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sTableSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sTableName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sConstSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sConstSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_CHAR,
                            sConstName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sConstInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            6,
                            SQL_C_CHAR,
                            sColumnName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sColumnInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            7,
                            SQL_C_CHAR,
                            sDeferrable,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sDeferrableInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            8,
                            SQL_C_CHAR,
                            sInitDeferred,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sInitDeferredInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * ALTER TABLE 구문 생성
     ************************************************************/

    /**
     * SET SESSION AUTHORIZATION owner
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"%s\"; ",
                 sConstOwnerName );
    
    /**
     * ALTER TABLE
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nALTER TABLE \"%s\".\"%s\" ",
                 aStringDDL,
                 sTableSchemaName,
                 sTableName );

    /**
     * ALTER COLUMN
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    ALTER COLUMN \"%s\" ",
                 aStringDDL,
                 sColumnName );

    /**
     * SET CONSTRAINT .. NOT NULL
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    SET CONSTRAINT \"%s\".\"%s\" NOT NULL ",
                 aStringDDL,
                 sConstSchemaName,
                 sConstName );

    /**
     * DEFERRABLE / NOT DEFERRABLE
     */
    
    if ( stlStrcmp( sDeferrable, "YES" ) == 0 )
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    DEFERRABLE ",
                     aStringDDL );
    }
    else
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    NOT DEFERRABLE ",
                     aStringDDL );
    }

    /**
     * INITIALLY IMMEDIATE / INITIALLY DEFERRED
     */
    
    if ( stlStrcmp( sInitDeferred, "YES" ) == 0 )
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    INITIALLY DEFERRED ",
                     aStringDDL );
    }
    else
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    INITIALLY IMMEDIATE ",
                     aStringDDL );
    }
    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintUniquePrimaryConstraint( stlInt64        aConstID,
                                           stlChar       * aStringDDL,
                                           stlAllocator  * aAllocator,
                                           stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlChar     sConstOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sConstOwnerInd;
    stlChar     sConstSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sConstSchemaInd;
    stlChar     sConstName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sConstInd;
    stlChar     sConstType[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sConstTypeInd;

    stlChar     sTableSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sTableSchemaInd;
    stlChar     sTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sTableInd;

    stlChar     sIndexSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIndexSchemaInd;
    stlChar     sIndexName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIndexInd;
    stlChar     sIsLogging[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsLoggingInd;
    stlChar     sSpaceName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSpaceInd;

    stlInt64    sPctFree;
    SQLLEN      sPctFreeInd;
    stlInt64    sIniTrans;
    SQLLEN      sIniTransInd;
    stlInt64    sMaxTrans;
    SQLLEN      sMaxTransInd;
    stlInt64    sSegInit;
    SQLLEN      sSegInitInd;
    stlInt64    sSegNext;
    SQLLEN      sSegNextInd;
    stlInt64    sSegMin;
    SQLLEN      sSegMinInd;
    stlInt64    sSegMax;
    SQLLEN      sSegMaxInd;

    stlChar     sDeferrable[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sDeferrableInd;
    stlChar     sInitDeferred[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sInitDeferredInd;
    
    stlInt32    sColCount = 0;
    stlChar     sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sColumnInd;
    stlChar     sIsAsc[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsAscInd;
    stlChar     sIsNullsFirst[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsNullsFirstInd;

    /************************************************************
     * ALTER TABLE 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME  "
                 "      , tsch.SCHEMA_NAME         "
                 "      , tab.TABLE_NAME           "
                 "      , csch.SCHEMA_NAME         "
                 "      , cst.CONSTRAINT_NAME      "
                 "      , cst.CONSTRAINT_TYPE      "
                 "      , isch.SCHEMA_NAME         "
                 "      , idx.INDEX_NAME           "
                 "      , xidx.PCTFREE             "
                 "      , xidx.INITRANS            "
                 "      , xidx.MAXTRANS            "
                 "      , ( xseg.INITIAL_EXTENTS * xspc.EXTSIZE ) "
                 "      , ( xseg.NEXT_EXTENTS * xspc.EXTSIZE )    "
                 "      , ( xseg.MIN_EXTENTS * xspc.EXTSIZE )     "
                 "      , ( xseg.MAX_EXTENTS * xspc.EXTSIZE )     "
                 "      , CAST( CASE WHEN xidx.IS_LOGGING = TRUE "
                 "                   THEN 'YES' "
                 "                   ELSE 'NO' "
                 "                   END "
                 "               AS VARCHAR(3 OCTETS) ) "
                 "      , spc.TABLESPACE_NAME      "
                 "      , CAST ( CASE WHEN cst.IS_DEFERRABLE = TRUE THEN 'YES' "
                 "                                                  ELSE 'NO' "
                 "                     END AS VARCHAR(3 OCTETS) ) "
                 "      , CAST ( CASE WHEN cst.INITIALLY_DEFERRED = TRUE THEN 'YES' "
                 "                                                       ELSE 'NO' "
                 "                     END AS VARCHAR(3 OCTETS) ) "
                 "   FROM "
                 "        DEFINITION_SCHEMA.TABLE_CONSTRAINTS     AS cst "
                 "      , DEFINITION_SCHEMA.TABLES                AS tab  "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS csch  "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS tsch  "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS isch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS        AS auth  "
                 "      , DEFINITION_SCHEMA.INDEXES               AS idx "
                 "      , DEFINITION_SCHEMA.TABLESPACES           AS spc   "
                 "      , FIXED_TABLE_SCHEMA.X$TABLESPACE         AS xspc   "
                 "      , FIXED_TABLE_SCHEMA.X$INDEX_HEADER       AS xidx "
                 "      , FIXED_TABLE_SCHEMA.X$SEGMENT            AS xseg "
                 "  WHERE "
                 "        cst.CONSTRAINT_SCHEMA_ID = csch.SCHEMA_ID "
                 "    AND cst.CONSTRAINT_OWNER_ID  = auth.AUTH_ID "
                 "    AND cst.TABLE_SCHEMA_ID      = tsch.SCHEMA_ID "
                 "    AND cst.TABLE_ID             = tab.TABLE_ID "
                 "    AND cst.ASSOCIATED_INDEX_ID  = idx.INDEX_ID "
                 "    AND idx.SCHEMA_ID            = isch.SCHEMA_ID "
                 "    AND idx.TABLESPACE_ID        = spc.TABLESPACE_ID "
                 "    AND idx.TABLESPACE_ID        = xspc.ID "
                 "    AND idx.PHYSICAL_ID          = xidx.PHYSICAL_ID "
                 "    AND idx.PHYSICAL_ID          = xseg.PHYSICAL_ID "
                 "    AND cst.CONSTRAINT_ID        = %ld ",
                 aConstID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sConstOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sConstOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sTableSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sTableSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sTableName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sConstSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sConstSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_CHAR,
                            sConstName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sConstInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            6,
                            SQL_C_CHAR,
                            sConstType,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sConstTypeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            7,
                            SQL_C_CHAR,
                            sIndexSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIndexSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            8,
                            SQL_C_CHAR,
                            sIndexName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIndexInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            9,
                            SQL_C_SBIGINT,
                            & sPctFree,
                            STL_SIZEOF(sPctFree),
                            & sPctFreeInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            10,
                            SQL_C_SBIGINT,
                            & sIniTrans,
                            STL_SIZEOF(sIniTrans),
                            & sIniTransInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            11,
                            SQL_C_SBIGINT,
                            & sMaxTrans,
                            STL_SIZEOF(sMaxTrans),
                            & sMaxTransInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            12,
                            SQL_C_SBIGINT,
                            & sSegInit,
                            STL_SIZEOF(sSegInit),
                            & sSegInitInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            13,
                            SQL_C_SBIGINT,
                            & sSegNext,
                            STL_SIZEOF(sSegNext),
                            & sSegNextInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            14,
                            SQL_C_SBIGINT,
                            & sSegMin,
                            STL_SIZEOF(sSegMin),
                            & sSegMinInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            15,
                            SQL_C_SBIGINT,
                            & sSegMax,
                            STL_SIZEOF(sSegMax),
                            & sSegMaxInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            16,
                            SQL_C_CHAR,
                            sIsLogging,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsLoggingInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            17,
                            SQL_C_CHAR,
                            sSpaceName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSpaceInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            18,
                            SQL_C_CHAR,
                            sDeferrable,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sDeferrableInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            19,
                            SQL_C_CHAR,
                            sInitDeferred,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sInitDeferredInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * ALTER TABLE 구문의 header 생성
     ************************************************************/

    /**
     * SET SESSION AUTHORIZATION owner
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"%s\"; ",
                 sConstOwnerName );
    
    /**
     * ALTER TABLE
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nALTER TABLE \"%s\".\"%s\" ",
                 aStringDDL,
                 sTableSchemaName,
                 sTableName );

    /**
     * ADD CONSTRAINT 
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    ADD CONSTRAINT \"%s\".\"%s\" ",
                 aStringDDL,
                 sConstSchemaName,
                 sConstName );

    /**
     * Constraint Type
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    %s ",
                 aStringDDL,
                 sConstType );

    /************************************************************
     * KEY COLUMN 정보 
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,

                 " SELECT "
                 "        col.COLUMN_NAME "
                 "      , CAST( CASE WHEN ikey.IS_ASCENDING_ORDER = TRUE "
                 "                   THEN 'ASC' "
                 "                   ELSE 'DESC' "
                 "                   END "
                 "                AS VARCHAR(32 OCTETS) ) "
                 "      , CAST( CASE WHEN ikey.IS_NULLS_FIRST = TRUE "
                 "                   THEN 'NULLS FIRST' "
                 "                   ELSE 'NULLS LAST' "
                 "                   END "
                 "                AS VARCHAR(32 OCTETS) ) "
                 "   FROM "
                 "        DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE AS ikey "
                 "      , DEFINITION_SCHEMA.TABLE_CONSTRAINTS      AS cst  "
                 "      , DEFINITION_SCHEMA.COLUMNS                AS col  "
                 "  WHERE      "
                 "        cst.ASSOCIATED_INDEX_ID = ikey.INDEX_ID  "
                 "    AND ikey.COLUMN_ID          = col.COLUMN_ID "
                 "    AND cst.CONSTRAINT_ID       = %ld "
                 "  ORDER BY "
                 "        ikey.ORDINAL_POSITION ",
                 aConstID );
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sColumnName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sColumnInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sIsAsc,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsAscInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sIsNullsFirst,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsNullsFirstInd,
                            aErrorStack )
             == STL_SUCCESS );

    /**
     * column 시작
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    ( ",
                 aStringDDL );

    sColCount = 0;
    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }
        
        /**
         * Column 이름과 Key 정보 출력
         */
        
        if ( sColCount == 0 )
        {
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n        \"%s\" %s %s",
                         aStringDDL,
                         sColumnName,
                         sIsAsc,
                         sIsNullsFirst );
        }
        else
        {
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n      , \"%s\" %s %s",
                         aStringDDL,
                         sColumnName,
                         sIsAsc,
                         sIsNullsFirst );
        }

        sColCount++;

    }

    /**
     * column 끝
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    ) ",
                 aStringDDL );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * ALTER TABLE 구문의 Tail 생성
     ************************************************************/

    /**
     * INDEX
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    INDEX \"%s\" ",
                 aStringDDL,
                 sIndexName );
    
    /**
     * index physical attribute
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    PCTFREE  %ld "
                 "\n    INITRANS %ld "
                 "\n    MAXTRANS %ld ",
                 aStringDDL,
                 sPctFree,
                 sIniTrans,
                 sMaxTrans );

    /**
     * segment attribute
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    STORAGE "
                 "\n    ( "
                 "\n        INITIAL %ld "
                 "\n        NEXT    %ld "
                 "\n        MINSIZE %ld "
                 "\n        MAXSIZE %ld "
                 "\n    ) ",
                 aStringDDL,
                 sSegInit,
                 sSegNext,
                 sSegMin,
                 sSegMax );

    /**
     * LOGGING 여부
     */

    if ( stlStrcmp( sIsLogging, "YES" ) == 0 )
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    LOGGING ",
                     aStringDDL );
    }
    else
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    NOLOGGING ",
                     aStringDDL );
    }
    
    /**
     * TABLESPACE tablespace 
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    TABLESPACE \"%s\" ",
                 aStringDDL,
                 sSpaceName );

    /**
     * DEFERRABLE / NOT DEFERRABLE
     */
    
    if ( stlStrcmp( sDeferrable, "YES" ) == 0 )
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    DEFERRABLE ",
                     aStringDDL );
    }
    else
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    NOT DEFERRABLE ",
                     aStringDDL );
    }

    /**
     * INITIALLY IMMEDIATE / INITIALLY DEFERRED
     */
    
    if ( stlStrcmp( sInitDeferred, "YES" ) == 0 )
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    INITIALLY DEFERRED ",
                     aStringDDL );
    }
    else
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    INITIALLY IMMEDIATE ",
                     aStringDDL );
    }
    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztqPrintCommentConstraint( stlInt64        aConstID,
                                     stlChar       * aStringDDL,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sConstName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sConstInd;
    stlChar     sComment[ZTQ_MAX_COMMENT_LENGTH];
    SQLLEN      sCommentInd;

    /************************************************************
     * COMMENT 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME  "
                 "      , sch.SCHEMA_NAME          "
                 "      , cst.CONSTRAINT_NAME      "
                 "      , cst.COMMENTS             "
                 "   FROM "
                 "        DEFINITION_SCHEMA.TABLE_CONSTRAINTS     AS cst  "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS        AS auth  "
                 "  WHERE "
                 "        cst.CONSTRAINT_SCHEMA_ID  = sch.SCHEMA_ID "
                 "    AND cst.CONSTRAINT_OWNER_ID   = auth.AUTH_ID "
                 "    AND cst.CONSTRAINT_ID         = %ld ",
                 aConstID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sConstName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sConstInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sComment,
                            ZTQ_MAX_COMMENT_LENGTH,
                            & sCommentInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * COMMENT ON CONSTRAINT 구문 생성
     ************************************************************/

    if ( sCommentInd == SQL_NULL_DATA )
    {
        /* comment 가 없는 경우 */
    }
    else
    {
        /**
         * SET SESSION AUTHORIZATION owner
         */
    
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "\nSET SESSION AUTHORIZATION \"%s\"; ",
                     sOwnerName );
    
        /**
         * COMMENT
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\nCOMMENT "
                     "\n    ON CONSTRAINT \"%s\".\"%s\" ",
                     aStringDDL,
                     sSchemaName,
                     sConstName );

        /**
         * IS 'comment'
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    IS '%s' ",
                     aStringDDL,
                     sComment );
    
        STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}






/****************************************************************************
 * Index
 ****************************************************************************/

stlStatus ztqGetIndexID( ztqIdentifier * aIdentifierList,
                         stlAllocator  * aAllocator,
                         stlInt64      * aIndexID,
                         stlErrorStack * aErrorStack )
{
    stlChar  * sIdentifier = NULL;
    stlInt32   sLen = 0;

    stlBool     sIsQualified = STL_FALSE;
    stlChar     sIndexName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    /************************************************************
     * double-quoted identifier 처리 
     ************************************************************/

    if ( aIdentifierList->mNext == NULL )
    {
        /**
         * index_name 만 기술한 경우 
         */

        sIsQualified = STL_FALSE;
        sIdentifier = aIdentifierList->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sIndexName, sIdentifier + 1, sLen - 2 );
            sIndexName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sIndexName, sIdentifier );
        }
    }
    else
    {
        /**
         * schema_name.index_name 으로  기술한 경우 
         */
        
        sIsQualified = STL_TRUE;

        sIdentifier = aIdentifierList->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sSchemaName, sIdentifier + 1, sLen - 2 );
            sSchemaName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sSchemaName, sIdentifier );
        }

        sIdentifier = ((ztqIdentifier *) aIdentifierList->mNext)->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sIndexName, sIdentifier + 1, sLen - 2 );
            sIndexName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sIndexName, sIdentifier );
        }
    }

    /************************************************************
     * Index ID 획득
     ************************************************************/

    if ( sIsQualified == STL_TRUE )
    {
        STL_TRY( ztqGetQualifiedIndexID( sSchemaName,
                                         sIndexName,
                                         aAllocator,
                                         aIndexID,
                                         aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztqGetNonQualifiedIndexID( sIndexName,
                                            aAllocator,
                                            aIndexID,
                                            aErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqGetQualifiedIndexID( stlChar       * aSchemaName,
                                  stlChar       * aIndexName,
                                  stlAllocator  * aAllocator,
                                  stlInt64      * aIndexID,
                                  stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sIndexID;
    SQLLEN      sIndexInd;
    
    /************************************************************
     * INDEX ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT idx.INDEX_ID "
                 "  FROM DEFINITION_SCHEMA.INDEXES idx "
                 "     , DEFINITION_SCHEMA.SCHEMATA sch "
                 " WHERE idx.SCHEMA_ID = sch.SCHEMA_ID "
                 "   AND idx.BY_CONSTRAINT = FALSE "
                 "   AND idx.INDEX_NAME = '%s' "
                 "   AND sch.SCHEMA_NAME = '%s' ",
                 aIndexName,
                 aSchemaName );

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sIndexID,
                            STL_SIZEOF(sIndexID),
                            & sIndexInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aIndexID = sIndexID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqGetNonQualifiedIndexID( stlChar       * aIndexName,
                                     stlAllocator  * aAllocator,
                                     stlInt64      * aIndexID,
                                     stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sIndexID;
    SQLLEN      sIndexInd;
    
    /************************************************************
     * INDEX ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        idx.INDEX_ID "
                 "   FROM "
                 "        DEFINITION_SCHEMA.SCHEMATA         AS sch "
                 "      , DEFINITION_SCHEMA.INDEXES          AS idx "
                 "      , ( "
                 "         ( SELECT "
                 "                sch1.SCHEMA_ID  "
                 "              , path1.SEARCH_ORDER  "
                 "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path1 "
                 "              , DEFINITION_SCHEMA.SCHEMATA         AS sch1 "
                 "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr1 "
                 "          WHERE "
                 "                usr1.AUTHORIZATION_NAME = CURRENT_USER "
                 "            AND path1.SCHEMA_ID         = sch1.SCHEMA_ID "
                 "            AND path1.AUTH_ID           = usr1.AUTH_ID "
                 "          ORDER BY "
                 "                path1.SEARCH_ORDER "
                 "         ) "
                 "       UNION ALL "
                 "         ( SELECT "
                 "                sch2.SCHEMA_ID "
                 "              , ( SELECT COUNT(*) "
                 "                    FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH "
                 "                   WHERE AUTH_ID = USER_ID() ) "
                 "                + path2.SEARCH_ORDER "
                 "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path2 "
                 "              , DEFINITION_SCHEMA.SCHEMATA         AS sch2  "
                 "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr2 "
                 "          WHERE "
                 "                usr2.AUTHORIZATION_NAME = 'PUBLIC' "
                 "            AND path2.SCHEMA_ID         = sch2.SCHEMA_ID "
                 "            AND path2.AUTH_ID           = usr2.AUTH_ID "
                 "          ORDER BY "
                 "                path2.SEARCH_ORDER "
                 "         ) "
                 "       ) AS upath ( SCHEMA_ID, SEARCH_ORDER ) "
                 " WHERE idx.SCHEMA_ID = sch.SCHEMA_ID "
                 "   AND sch.SCHEMA_ID = upath.SCHEMA_ID "
                 "   AND idx.BY_CONSTRAINT = FALSE "
                 "   AND idx.INDEX_NAME = '%s' "
                 " ORDER BY "
                 "       upath.SEARCH_ORDER  "
                 " FETCH 1 ",
                 aIndexName );
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sIndexID,
                            STL_SIZEOF(sIndexID),
                            & sIndexInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aIndexID = sIndexID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintCreateIndex( stlInt64        aIndexID,
                               stlChar       * aStringDDL,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sIsUnique[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsUniqueInd;
    stlChar     sIndexSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIndexSchemaInd;
    stlChar     sIndexName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIndexInd;
    stlChar     sTableSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sTableSchemaInd;
    stlChar     sTableName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sTableInd;
    stlChar     sIsLogging[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsLoggingInd;
    stlChar     sSpaceName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSpaceInd;

    stlInt64    sPctFree;
    SQLLEN      sPctFreeInd;
    stlInt64    sIniTrans;
    SQLLEN      sIniTransInd;
    stlInt64    sMaxTrans;
    SQLLEN      sMaxTransInd;
    stlInt64    sSegInit;
    SQLLEN      sSegInitInd;
    stlInt64    sSegNext;
    SQLLEN      sSegNextInd;
    stlInt64    sSegMin;
    SQLLEN      sSegMinInd;
    stlInt64    sSegMax;
    SQLLEN      sSegMaxInd;
    
    stlInt32    sColCount = 0;
    stlChar     sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sColumnInd;
    stlChar     sIsAsc[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsAscInd;
    stlChar     sIsNullsFirst[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsNullsFirstInd;

    /************************************************************
     * CREATE INDEX 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth1.AUTHORIZATION_NAME  "
                 "      , CAST( CASE WHEN idx.IS_UNIQUE = TRUE  "
                 "                   THEN 'YES' "
                 "                   ELSE 'NO' "
                 "                   END "
                 "              AS VARCHAR(3 OCTETS) )  "
                 "      , sch1.SCHEMA_NAME         "
                 "      , idx.INDEX_NAME           "
                 "      , sch2.SCHEMA_NAME         "
                 "      , tab.TABLE_NAME           "
                 "      , xidx.PCTFREE             "
                 "      , xidx.INITRANS            "
                 "      , xidx.MAXTRANS            "
                 "      , ( xseg.INITIAL_EXTENTS * xspc.EXTSIZE ) "
                 "      , ( xseg.NEXT_EXTENTS * xspc.EXTSIZE )    "
                 "      , ( xseg.MIN_EXTENTS * xspc.EXTSIZE )     "
                 "      , ( xseg.MAX_EXTENTS * xspc.EXTSIZE )     "
                 "      , CAST( CASE WHEN xidx.IS_LOGGING = TRUE "
                 "                   THEN 'YES' "
                 "                   ELSE 'NO' "
                 "                   END "
                 "               AS VARCHAR(3 OCTETS) ) "
                 "      , spc.TABLESPACE_NAME       "
                 "   FROM "
                 "        DEFINITION_SCHEMA.INDEXES               AS idx "
                 "      , DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE AS ikey "
                 "      , DEFINITION_SCHEMA.TABLES                AS tab  "
                 "      , DEFINITION_SCHEMA.TABLESPACES           AS spc   "
                 "      , FIXED_TABLE_SCHEMA.X$TABLESPACE         AS xspc   "
                 "      , FIXED_TABLE_SCHEMA.X$INDEX_HEADER       AS xidx "
                 "      , FIXED_TABLE_SCHEMA.X$SEGMENT            AS xseg "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS sch1  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS        AS auth1  "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS sch2  "
                 "  WHERE "
                 "        idx.INDEX_ID           = ikey.INDEX_ID "
                 "    AND ikey.TABLE_ID          = tab.TABLE_ID "
                 "    AND idx.TABLESPACE_ID      = spc.TABLESPACE_ID "
                 "    AND idx.TABLESPACE_ID      = xspc.ID "
                 "    AND idx.PHYSICAL_ID        = xidx.PHYSICAL_ID "
                 "    AND idx.PHYSICAL_ID        = xseg.PHYSICAL_ID "
                 "    AND ikey.INDEX_SCHEMA_ID   = sch1.SCHEMA_ID "
                 "    AND ikey.INDEX_OWNER_ID    = auth1.AUTH_ID "
                 "    AND ikey.TABLE_SCHEMA_ID   = sch2.SCHEMA_ID "
                 "    AND idx.INDEX_ID           = %ld ",
                 aIndexID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sIsUnique,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsUniqueInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sIndexSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIndexSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sIndexName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIndexInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_CHAR,
                            sTableSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sTableSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            6,
                            SQL_C_CHAR,
                            sTableName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sTableInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            7,
                            SQL_C_SBIGINT,
                            & sPctFree,
                            STL_SIZEOF(sPctFree),
                            & sPctFreeInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            8,
                            SQL_C_SBIGINT,
                            & sIniTrans,
                            STL_SIZEOF(sIniTrans),
                            & sIniTransInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            9,
                            SQL_C_SBIGINT,
                            & sMaxTrans,
                            STL_SIZEOF(sMaxTrans),
                            & sMaxTransInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            10,
                            SQL_C_SBIGINT,
                            & sSegInit,
                            STL_SIZEOF(sSegInit),
                            & sSegInitInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            11,
                            SQL_C_SBIGINT,
                            & sSegNext,
                            STL_SIZEOF(sSegNext),
                            & sSegNextInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            12,
                            SQL_C_SBIGINT,
                            & sSegMin,
                            STL_SIZEOF(sSegMin),
                            & sSegMinInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            13,
                            SQL_C_SBIGINT,
                            & sSegMax,
                            STL_SIZEOF(sSegMax),
                            & sSegMaxInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            14,
                            SQL_C_CHAR,
                            sIsLogging,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsLoggingInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            15,
                            SQL_C_CHAR,
                            sSpaceName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSpaceInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * CREATE INDEX 구문의 Header 생성
     ************************************************************/

    /**
     * SET SESSION AUTHORIZATION grantor
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"%s\"; ",
                 sOwnerName );
    
    /**
     * CREATE INDEX
     */

    if ( stlStrcmp( sIsUnique, "YES" ) == 0 )
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\nCREATE UNIQUE INDEX \"%s\".\"%s\" ",
                     aStringDDL,
                     sIndexSchemaName,
                     sIndexName );
    }
    else
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\nCREATE INDEX \"%s\".\"%s\" ",
                     aStringDDL,
                     sIndexSchemaName,
                     sIndexName );
    }

    /**
     * ON table
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    ON \"%s\".\"%s\" ",
                 aStringDDL,
                 sTableSchemaName,
                 sTableName );
    
    /************************************************************
     * Column 정보 생성
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,

                 " SELECT "
                 "        col.COLUMN_NAME "
                 "      , CAST( CASE WHEN ikey.IS_ASCENDING_ORDER = TRUE "
                 "                   THEN 'ASC' "
                 "                   ELSE 'DESC' "
                 "                   END "
                 "                AS VARCHAR(32 OCTETS) ) "
                 "      , CAST( CASE WHEN ikey.IS_NULLS_FIRST = TRUE "
                 "                   THEN 'NULLS FIRST' "
                 "                   ELSE 'NULLS LAST' "
                 "                   END "
                 "                AS VARCHAR(32 OCTETS) ) "
                 "   FROM "
                 "        DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE AS ikey "
                 "      , DEFINITION_SCHEMA.INDEXES                AS idx  "
                 "      , DEFINITION_SCHEMA.COLUMNS                AS col  "
                 "  WHERE      "
                 "        ikey.INDEX_ID  = idx.INDEX_ID  "
                 "    AND ikey.COLUMN_ID = col.COLUMN_ID "
                 "    AND idx.INDEX_ID   = %ld "
                 "  ORDER BY "
                 "        ikey.ORDINAL_POSITION ",
                 aIndexID );
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sColumnName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sColumnInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sIsAsc,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsAscInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sIsNullsFirst,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsNullsFirstInd,
                            aErrorStack )
             == STL_SUCCESS );

    /**
     * column 시작
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    ( ",
                 aStringDDL );

    sColCount = 0;
    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        if ( sResult == SQL_NO_DATA )
        {
            break;
        }
        
        /**
         * Column 이름과 Key 정보 출력
         */
        
        if ( sColCount == 0 )
        {
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n        \"%s\" %s %s",
                         aStringDDL,
                         sColumnName,
                         sIsAsc,
                         sIsNullsFirst );
        }
        else
        {
            stlSnprintf( aStringDDL,
                         gZtqDdlSize,
                         "%s"
                         "\n      , \"%s\" %s %s",
                         aStringDDL,
                         sColumnName,
                         sIsAsc,
                         sIsNullsFirst );
        }

        sColCount++;

    }

    /**
     * column 끝
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    ) ",
                 aStringDDL );
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    

    /************************************************************
     * CREATE INDEX 구문의 Tail 생성
     ************************************************************/

    /**
     * index physical attribute
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    PCTFREE  %ld "
                 "\n    INITRANS %ld "
                 "\n    MAXTRANS %ld ",
                 aStringDDL,
                 sPctFree,
                 sIniTrans,
                 sMaxTrans );

    /**
     * segment attribute
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    STORAGE "
                 "\n    ( "
                 "\n        INITIAL %ld "
                 "\n        NEXT    %ld "
                 "\n        MINSIZE %ld "
                 "\n        MAXSIZE %ld "
                 "\n    ) ",
                 aStringDDL,
                 sSegInit,
                 sSegNext,
                 sSegMin,
                 sSegMax );

    /**
     * LOGGING 여부
     */

    if ( stlStrcmp( sIsLogging, "YES" ) == 0 )
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    LOGGING ",
                     aStringDDL );
    }
    else
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    NOLOGGING ",
                     aStringDDL );
    }
    
    /**
     * TABLESPACE tablespace 
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    TABLESPACE \"%s\" ",
                 aStringDDL,
                 sSpaceName );
    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus ztqPrintCommentIndex( stlInt64        aIndexID,
                                stlChar       * aStringDDL,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sIndexName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIndexInd;
    stlChar     sComment[ZTQ_MAX_COMMENT_LENGTH];
    SQLLEN      sCommentInd;

    /************************************************************
     * COMMENT 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME  "
                 "      , sch.SCHEMA_NAME          "
                 "      , idx.INDEX_NAME           "
                 "      , idx.COMMENTS             "
                 "   FROM "
                 "        DEFINITION_SCHEMA.INDEXES               AS idx  "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS        AS auth  "
                 "  WHERE "
                 "        idx.SCHEMA_ID   = sch.SCHEMA_ID "
                 "    AND idx.OWNER_ID    = auth.AUTH_ID "
                 "    AND idx.INDEX_ID    = %ld ",
                 aIndexID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sIndexName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIndexInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sComment,
                            ZTQ_MAX_COMMENT_LENGTH,
                            & sCommentInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * COMMENT ON INDEX 구문 생성
     ************************************************************/

    if ( sCommentInd == SQL_NULL_DATA )
    {
        /* comment 가 없는 경우 */
    }
    else
    {
        /**
         * SET SESSION AUTHORIZATION owner
         */
    
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "\nSET SESSION AUTHORIZATION \"%s\"; ",
                     sOwnerName );
    
        /**
         * COMMENT
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\nCOMMENT "
                     "\n    ON INDEX \"%s\".\"%s\" ",
                     aStringDDL,
                     sSchemaName,
                     sIndexName );

        /**
         * IS 'comment'
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    IS '%s' ",
                     aStringDDL,
                     sComment );
    
        STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}





/****************************************************************************
 * View
 ****************************************************************************/


stlStatus ztqGetViewID( ztqIdentifier * aIdentifierList,
                        stlAllocator  * aAllocator,
                        stlInt64      * aViewID,
                        stlErrorStack * aErrorStack )
{
    stlChar  * sIdentifier = NULL;
    stlInt32   sLen = 0;

    stlBool     sIsQualified = STL_FALSE;
    stlChar     sViewName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    /************************************************************
     * double-quoted identifier 처리 
     ************************************************************/

    if ( aIdentifierList->mNext == NULL )
    {
        /**
         * view_name 만 기술한 경우 
         */

        sIsQualified = STL_FALSE;
        sIdentifier = aIdentifierList->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sViewName, sIdentifier + 1, sLen - 2 );
            sViewName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sViewName, sIdentifier );
        }
    }
    else
    {
        /**
         * schema_name.table_name 으로  기술한 경우 
         */
        
        sIsQualified = STL_TRUE;

        sIdentifier = aIdentifierList->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sSchemaName, sIdentifier + 1, sLen - 2 );
            sSchemaName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sSchemaName, sIdentifier );
        }

        sIdentifier = ((ztqIdentifier *) aIdentifierList->mNext)->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sViewName, sIdentifier + 1, sLen - 2 );
            sViewName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sViewName, sIdentifier );
        }
    }

    /************************************************************
     * View ID 획득
     ************************************************************/

    if ( sIsQualified == STL_TRUE )
    {
        STL_TRY( ztqGetQualifiedViewID( sSchemaName,
                                        sViewName,
                                        aAllocator,
                                        aViewID,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztqGetNonQualifiedViewID( sViewName,
                                           aAllocator,
                                           aViewID,
                                           aErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqGetQualifiedViewID( stlChar       * aSchemaName,
                                 stlChar       * aViewName,
                                 stlAllocator  * aAllocator,
                                 stlInt64      * aViewID,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sViewID;
    SQLLEN      sViewInd;
    
    /************************************************************
     * TABLE ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT tab.TABLE_ID "
                 "  FROM DEFINITION_SCHEMA.TABLES tab "
                 "     , DEFINITION_SCHEMA.SCHEMATA sch "
                 " WHERE tab.SCHEMA_ID = sch.SCHEMA_ID "
                 "   AND tab.TABLE_TYPE = 'VIEW' "
                 "   AND tab.TABLE_NAME = '%s' "
                 "   AND sch.SCHEMA_NAME = '%s' ",
                 aViewName,
                 aSchemaName );

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sViewID,
                            STL_SIZEOF(sViewID),
                            & sViewInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aViewID = sViewID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqGetNonQualifiedViewID( stlChar       * aViewName,
                                    stlAllocator  * aAllocator,
                                    stlInt64      * aViewID,
                                    stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sViewID;
    SQLLEN      sViewInd;
    
    /************************************************************
     * TABLE ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        tab.TABLE_ID "
                 "   FROM "
                 "        DEFINITION_SCHEMA.SCHEMATA         AS sch "
                 "      , DEFINITION_SCHEMA.TABLES           AS tab "
                 "      , ( "
                 "         ( SELECT "
                 "                sch1.SCHEMA_ID  "
                 "              , path1.SEARCH_ORDER  "
                 "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path1 "
                 "              , DEFINITION_SCHEMA.SCHEMATA         AS sch1 "
                 "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr1 "
                 "          WHERE "
                 "                usr1.AUTHORIZATION_NAME = CURRENT_USER "
                 "            AND path1.SCHEMA_ID         = sch1.SCHEMA_ID "
                 "            AND path1.AUTH_ID           = usr1.AUTH_ID "
                 "          ORDER BY "
                 "                path1.SEARCH_ORDER "
                 "         ) "
                 "       UNION ALL "
                 "         ( SELECT "
                 "                sch2.SCHEMA_ID "
                 "              , ( SELECT COUNT(*) "
                 "                    FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH "
                 "                   WHERE AUTH_ID = USER_ID() ) "
                 "                + path2.SEARCH_ORDER "
                 "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path2 "
                 "              , DEFINITION_SCHEMA.SCHEMATA         AS sch2  "
                 "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr2 "
                 "          WHERE "
                 "                usr2.AUTHORIZATION_NAME = 'PUBLIC' "
                 "            AND path2.SCHEMA_ID         = sch2.SCHEMA_ID "
                 "            AND path2.AUTH_ID           = usr2.AUTH_ID "
                 "          ORDER BY "
                 "                path2.SEARCH_ORDER "
                 "         ) "
                 "       ) AS upath ( SCHEMA_ID, SEARCH_ORDER ) "
                 " WHERE tab.SCHEMA_ID = sch.SCHEMA_ID "
                 "   AND sch.SCHEMA_ID = upath.SCHEMA_ID "
                 "   AND tab.TABLE_TYPE = 'VIEW' "
                 "   AND tab.TABLE_NAME = '%s' "
                 " ORDER BY "
                 "       upath.SEARCH_ORDER  "
                 " FETCH 1 ",
                 aViewName );
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sViewID,
                            STL_SIZEOF(sViewID),
                            & sViewInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aViewID = sViewID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintCreateView( stlInt64        aViewID,
                              stlChar       * aStringDDL,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sViewName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sViewInd;

    stlChar   * sViewColumnLong;
    SQLLEN      sViewColumnInd;
    stlChar   * sViewSelectLong;
    SQLLEN      sViewSelectInd;
    
    stlRegionMark   sMark;
    stlBool         sIsMarked = STL_FALSE;
    
    /**
     * DEFAULT 공간 확보
     */

    STL_TRY( stlMarkRegionMem( aAllocator,
                               &sMark,
                               aErrorStack ) == STL_SUCCESS );

    sIsMarked = STL_TRUE;
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sViewColumnLong,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sViewSelectLong,
                                aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * CREATE VIEW 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME   "
                 "      , sch.SCHEMA_NAME           "
                 "      , tab.TABLE_NAME            "
                 "      , SPLIT_PART( SPLIT_PART( vew.VIEW_COLUMNS, ')', 1 ), '(', 2 ) "
                 "      , vew.VIEW_DEFINITION       "
                 "   FROM "
                 "        DEFINITION_SCHEMA.TABLES           AS tab  "
                 "      , DEFINITION_SCHEMA.VIEWS            AS vew  "
                 "      , DEFINITION_SCHEMA.SCHEMATA         AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS   AS auth  "
                 "  WHERE "
                 "        tab.TABLE_ID   = vew.TABLE_ID "
                 "    AND tab.SCHEMA_ID  = sch.SCHEMA_ID "
                 "    AND tab.OWNER_ID   = auth.AUTH_ID "
                 "    AND tab.TABLE_ID   = %ld ",
                 aViewID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sViewName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sViewInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sViewColumnLong,
                            gZtqDdlSize,
                            & sViewColumnInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_CHAR,
                            sViewSelectLong,
                            gZtqDdlSize,
                            & sViewSelectInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );

    switch ( sResult )
    {
        case SQL_SUCCESS:
            {
                /* ok */
                break;
            }
        case SQL_SUCCESS_WITH_INFO:
            {
                if ( (sViewColumnInd >= gZtqDdlSize) || (sViewSelectInd >= gZtqDdlSize) )
                {
                    STL_THROW( RAMP_ERR_NOT_ENOUGH_DDLSIZE );
                }
                else
                {
                    /**
                     * ztqSQLFetch() 에서 메시지를 출력함
                     */
                    STL_TRY( 0 );
                }
            
                break;
            }
        case SQL_NO_DATA:
            {
                STL_THROW( RAMP_ERR_INVALID_OBJECT_IDENTIFIER );
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * CREATE VIEW 구문의 Header 생성
     ************************************************************/

    /**
     * SET SESSION AUTHORIZATION grantor
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"%s\"; ",
                 sOwnerName );
    
    /**
     * CREATE VIEW
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nCREATE OR REPLACE FORCE VIEW \"%s\".\"%s\" ",
                 aStringDDL,
                 sSchemaName,
                 sViewName );

    if ( sViewColumnInd == SQL_NULL_DATA )
    {
        /* nothing to do */
    }
    else
    {
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    (%s) ",
                     aStringDDL,
                     sViewColumnLong );
    }

    /**
     * AS SELECT
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    AS %s",
                 aStringDDL,
                 sViewSelectLong );
    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        STL_TRY( stlRestoreRegionMem( aAllocator,
                                      &sMark,
                                      STL_FALSE, /* aFree */
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_NOT_ENOUGH_DDLSIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_ENOUGH_DDLSIZE,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        (void) stlRestoreRegionMem( aAllocator,
                                    &sMark,
                                    STL_FALSE, /* aFree */
                                    aErrorStack );
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintViewRecompile( stlInt64        aViewID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sViewName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sViewInd;

    /************************************************************
     * ALTER VIEW 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME   "
                 "      , sch.SCHEMA_NAME           "
                 "      , tab.TABLE_NAME            "
                 "   FROM "
                 "        DEFINITION_SCHEMA.TABLES           AS tab  "
                 "      , DEFINITION_SCHEMA.SCHEMATA         AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS   AS auth  "
                 "  WHERE "
                 "        tab.SCHEMA_ID  = sch.SCHEMA_ID "
                 "    AND tab.OWNER_ID   = auth.AUTH_ID "
                 "    AND tab.TABLE_ID   = %ld ",
                 aViewID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sViewName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sViewInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sResult == STL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    /************************************************************
     * ALTER VIEW 구문의 Header 생성
     ************************************************************/

    /**
     * SET SESSION AUTHORIZATION grantor
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"%s\"; ",
                 sOwnerName );
    
    /**
     * CREATE VIEW
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nALTER VIEW \"%s\".\"%s\" COMPILE",
                 aStringDDL,
                 sSchemaName,
                 sViewName );

    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}



/****************************************************************************
 * Sequence
 ****************************************************************************/

stlStatus ztqGetSequenceID( ztqIdentifier * aIdentifierList,
                            stlAllocator  * aAllocator,
                            stlInt64      * aSeqID,
                            stlErrorStack * aErrorStack )
{
    stlChar  * sIdentifier = NULL;
    stlInt32   sLen = 0;

    stlBool     sIsQualified = STL_FALSE;
    stlChar     sSeqName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    /************************************************************
     * double-quoted identifier 처리 
     ************************************************************/

    if ( aIdentifierList->mNext == NULL )
    {
        /**
         * sequence_name 만 기술한 경우 
         */

        sIsQualified = STL_FALSE;
        sIdentifier = aIdentifierList->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sSeqName, sIdentifier + 1, sLen - 2 );
            sSeqName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sSeqName, sIdentifier );
        }
    }
    else
    {
        /**
         * schema_name.index_name 으로  기술한 경우 
         */
        
        sIsQualified = STL_TRUE;

        sIdentifier = aIdentifierList->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sSchemaName, sIdentifier + 1, sLen - 2 );
            sSchemaName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sSchemaName, sIdentifier );
        }

        sIdentifier = ((ztqIdentifier *) aIdentifierList->mNext)->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sSeqName, sIdentifier + 1, sLen - 2 );
            sSeqName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sSeqName, sIdentifier );
        }
    }

    /************************************************************
     * Sequence ID 획득
     ************************************************************/

    if ( sIsQualified == STL_TRUE )
    {
        STL_TRY( ztqGetQualifiedSequenceID( sSchemaName,
                                            sSeqName,
                                            aAllocator,
                                            aSeqID,
                                            aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztqGetNonQualifiedSequenceID( sSeqName,
                                               aAllocator,
                                               aSeqID,
                                               aErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}



stlStatus ztqGetQualifiedSequenceID( stlChar       * aSchemaName,
                                     stlChar       * aSeqName,
                                     stlAllocator  * aAllocator,
                                     stlInt64      * aSeqID,
                                     stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSeqID;
    SQLLEN      sSeqInd;
    
    /************************************************************
     * SEQUENCE ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT seq.SEQUENCE_ID "
                 "  FROM DEFINITION_SCHEMA.SEQUENCES seq "
                 "     , DEFINITION_SCHEMA.SCHEMATA sch "
                 " WHERE seq.SCHEMA_ID = sch.SCHEMA_ID "
                 "   AND seq.SEQUENCE_NAME = '%s' "
                 "   AND sch.SCHEMA_NAME = '%s' ",
                 aSeqName,
                 aSchemaName );

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSeqID,
                            STL_SIZEOF(sSeqID),
                            & sSeqInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aSeqID = sSeqID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqGetNonQualifiedSequenceID( stlChar       * aSeqName,
                                        stlAllocator  * aAllocator,
                                        stlInt64      * aSeqID,
                                        stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSeqID;
    SQLLEN      sSeqInd;
    
    /************************************************************
     * SEQUENCE ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        seq.SEQUENCE_ID "
                 "   FROM "
                 "        DEFINITION_SCHEMA.SCHEMATA         AS sch "
                 "      , DEFINITION_SCHEMA.SEQUENCES        AS seq "
                 "      , ( "
                 "         ( SELECT "
                 "                sch1.SCHEMA_ID  "
                 "              , path1.SEARCH_ORDER  "
                 "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path1 "
                 "              , DEFINITION_SCHEMA.SCHEMATA         AS sch1 "
                 "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr1 "
                 "          WHERE "
                 "                usr1.AUTHORIZATION_NAME = CURRENT_USER "
                 "            AND path1.SCHEMA_ID         = sch1.SCHEMA_ID "
                 "            AND path1.AUTH_ID           = usr1.AUTH_ID "
                 "          ORDER BY "
                 "                path1.SEARCH_ORDER "
                 "         ) "
                 "       UNION ALL "
                 "         ( SELECT "
                 "                sch2.SCHEMA_ID "
                 "              , ( SELECT COUNT(*) "
                 "                    FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH "
                 "                   WHERE AUTH_ID = USER_ID() ) "
                 "                + path2.SEARCH_ORDER "
                 "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path2 "
                 "              , DEFINITION_SCHEMA.SCHEMATA         AS sch2  "
                 "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr2 "
                 "          WHERE "
                 "                usr2.AUTHORIZATION_NAME = 'PUBLIC' "
                 "            AND path2.SCHEMA_ID         = sch2.SCHEMA_ID "
                 "            AND path2.AUTH_ID           = usr2.AUTH_ID "
                 "          ORDER BY "
                 "                path2.SEARCH_ORDER "
                 "         ) "
                 "       ) AS upath ( SCHEMA_ID, SEARCH_ORDER ) "
                 " WHERE seq.SCHEMA_ID = sch.SCHEMA_ID "
                 "   AND sch.SCHEMA_ID = upath.SCHEMA_ID "
                 "   AND seq.SEQUENCE_NAME = '%s' "
                 " ORDER BY "
                 "       upath.SEARCH_ORDER  "
                 " FETCH 1 ",
                 aSeqName );
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSeqID,
                            STL_SIZEOF(sSeqID),
                            & sSeqInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aSeqID = sSeqID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintCreateSequence( stlInt64        aSeqID,
                                  stlChar       * aStringDDL,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sSeqName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSeqInd;

    stlInt64    sStart;
    SQLLEN      sStartInd;
    stlInt64    sIncrement;
    SQLLEN      sIncrementInd;
    stlInt64    sMaxVal;
    SQLLEN      sMaxValInd;
    stlInt64    sMinVal;
    SQLLEN      sMinValInd;
    stlChar     sIsCycle[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sIsCycleInd;
    stlInt64    sCache;
    SQLLEN      sCacheInd;
    

    /************************************************************
     * CREATE INDEX 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME  "
                 "      , sch.SCHEMA_NAME          "
                 "      , seq.SEQUENCE_NAME        "
                 "      , seq.START_VALUE          "
                 "      , seq.INCREMENT            "
                 "      , seq.MAXIMUM_VALUE        "
                 "      , seq.MINIMUM_VALUE        "
                 "      , CAST( CASE WHEN seq.CYCLE_OPTION = TRUE        "
                 "                   THEN 'CYCLE'        "
                 "                   ELSE 'NO CYCLE'     "
                 "                   END                 "
                 "              AS VARCHAR(32 OCTETS) )  "
                 "      , seq.CACHE_SIZE           "
                 "   FROM "
                 "        DEFINITION_SCHEMA.SEQUENCES             AS seq "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS        AS auth "
                 "  WHERE "
                 "        seq.SCHEMA_ID   = sch.SCHEMA_ID "
                 "    AND seq.OWNER_ID    = auth.AUTH_ID "
                 "    AND seq.SEQUENCE_ID = %ld ",
                 aSeqID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sSeqName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSeqInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_SBIGINT,
                            & sStart,
                            STL_SIZEOF(sStart),
                            & sStartInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_SBIGINT,
                            & sIncrement,
                            STL_SIZEOF(sIncrement),
                            & sIncrementInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            6,
                            SQL_C_SBIGINT,
                            & sMaxVal,
                            STL_SIZEOF(sMaxVal),
                            & sMaxValInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            7,
                            SQL_C_SBIGINT,
                            & sMinVal,
                            STL_SIZEOF(sMinVal),
                            & sMinValInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            8,
                            SQL_C_CHAR,
                            sIsCycle,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sIsCycleInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            9,
                            SQL_C_SBIGINT,
                            & sCache,
                            STL_SIZEOF(sCache),
                            & sCacheInd,
                            aErrorStack )
             == STL_SUCCESS );

    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * CREATE SEQUENCE 구문 생성
     ************************************************************/

    /**
     * SET SESSION AUTHORIZATION grantor
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"%s\"; ",
                 sOwnerName );
    
    /**
     * CREATE SEQUENCE
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nCREATE SEQUENCE \"%s\".\"%s\" ",
                 aStringDDL,
                 sSchemaName,
                 sSeqName );

    /**
     * sequence option
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    START WITH %ld "
                 "\n    INCREMENT BY %ld "
                 "\n    MAXVALUE %ld "
                 "\n    MINVALUE %ld "
                 "\n    %s "
                 "\n    CACHE %ld ",
                 aStringDDL,
                 sStart,
                 sIncrement,
                 sMaxVal,
                 sMinVal,
                 sIsCycle,
                 sCache );
    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintRestartSequence( stlInt64        aSeqID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sSeqName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSeqInd;

    stlInt64    sRestart;
    SQLLEN      sRestartInd;

    /************************************************************
     * CREATE INDEX 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME  "
                 "      , sch.SCHEMA_NAME          "
                 "      , seq.SEQUENCE_NAME        "
                 "      , xsqc.RESTART_VALUE       "
                 "   FROM "
                 "        DEFINITION_SCHEMA.SEQUENCES             AS seq "
                 "      , FIXED_TABLE_SCHEMA.X$SEQUENCE           AS xsqc "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS        AS auth "
                 "  WHERE "
                 "        seq.SCHEMA_ID   = sch.SCHEMA_ID "
                 "    AND seq.OWNER_ID    = auth.AUTH_ID "
                 "    AND seq.PHYSICAL_ID = xsqc.PHYSICAL_ID "
                 "    AND seq.SEQUENCE_ID = %ld ",
                 aSeqID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sSeqName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSeqInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_SBIGINT,
                            & sRestart,
                            STL_SIZEOF(sRestart),
                            & sRestartInd,
                            aErrorStack )
             == STL_SUCCESS );

    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * ALTER SEQUENCE 구문 생성
     ************************************************************/

    /**
     * SET SESSION AUTHORIZATION grantor
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"%s\"; ",
                 sOwnerName );
    
    /**
     * ALTER SEQUENCE
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nALTER SEQUENCE \"%s\".\"%s\" ",
                 aStringDDL,
                 sSchemaName,
                 sSeqName );

    /**
     * RESTART
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\n    RESTART WITH %ld ",
                 aStringDDL,
                 sRestart );
    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlStatus ztqPrintGrantSequence( stlInt64        aSeqID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlRegionMark   sMark;
    stlBool         sIsMarked = STL_FALSE;

    ztqPrivOrder  sPrivOrder;

    stlInt64    sOwnerID;
    SQLLEN      sOwnerInd;

    stlInt64    sGrantorID;
    SQLLEN      sGrantorInd;
    stlChar     sGrantorName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGrantorNameInd;
    
    stlInt64    sGranteeID;
    SQLLEN      sGranteeInd;
    stlChar     sGranteeName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGranteeNameInd;

    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sSeqName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSeqInd;

    stlChar     sGrantable[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sGrantInd;

    stlChar     sIsBuiltIn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlStrncpy( sIsBuiltIn, "NO", STL_MAX_SQL_IDENTIFIER_LENGTH );


    /**
     * Memory Mark 설정
     */
    
    STL_TRY( stlMarkRegionMem( aAllocator,
                               &sMark,
                               aErrorStack ) == STL_SUCCESS );
    sIsMarked = STL_TRUE;
    
    /************************************************************
     * GRANT SEQUENCE 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        seq.OWNER_ID "
                 "      , grantor.AUTH_ID "
                 "      , grantor.AUTHORIZATION_NAME  "
                 "      , grantee.AUTH_ID "
                 "      , grantee.AUTHORIZATION_NAME  "
                 "      , sch.SCHEMA_NAME          "
                 "      , seq.SEQUENCE_NAME        "
                 "      , CAST( CASE WHEN upriv.IS_GRANTABLE = TRUE THEN 'YES' "
                 "                                                  ELSE 'NO'  "
                 "                   END  "
                 "              AS VARCHAR(32 OCTETS) ) "
                 "   FROM "
                 "        DEFINITION_SCHEMA.SEQUENCES             AS seq "
                 "      , DEFINITION_SCHEMA.USAGE_PRIVILEGES      AS upriv "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS        AS grantor "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS        AS grantee "
                 "  WHERE "
                 "        grantor.AUTHORIZATION_NAME <> '_SYSTEM' "
                 "    AND grantee.AUTH_ID >= ( SELECT AUTH_ID "
                 "                               FROM DEFINITION_SCHEMA.AUTHORIZATIONS "
                 "                              WHERE AUTHORIZATION_NAME = 'PUBLIC' ) "
                 "    AND seq.SEQUENCE_ID   = upriv.OBJECT_ID "
                 "    AND seq.SCHEMA_ID     = sch.SCHEMA_ID "
                 "    AND upriv.GRANTOR_ID  = grantor.AUTH_ID "
                 "    AND upriv.GRANTEE_ID  = grantee.AUTH_ID "
                 "    AND seq.SEQUENCE_ID = %ld "
                 "  ORDER BY "
                 "        grantor.AUTH_ID "
                 "      , grantee.AUTH_ID ",
                 aSeqID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sOwnerID,
                            STL_SIZEOF(sOwnerID),
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_SBIGINT,
                            & sGrantorID,
                            STL_SIZEOF(sGrantorID),
                            & sGrantorInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sGrantorName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGrantorNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_SBIGINT,
                            & sGranteeID,
                            STL_SIZEOF(sGranteeID),
                            & sGranteeInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_CHAR,
                            sGranteeName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGranteeNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            6,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            7,
                            SQL_C_CHAR,
                            sSeqName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSeqInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            8,
                            SQL_C_CHAR,
                            sGrantable,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sGrantInd,
                            aErrorStack )
             == STL_SUCCESS );

    /**
     * 최초 Fetch
     */
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_TABLE_FINISH );

    /**
     * Privilege Print Order 결정을 위한 초기화
     */
    
    ztqInitPrivOrder( & sPrivOrder );

    STL_TRY( ztqAddPrivItem( & sPrivOrder,
                             sGrantorID,
                             sGrantorName,
                             sGranteeID,
                             sGranteeName,
                             sIsBuiltIn, 
                             sGrantable,
                             aAllocator,
                             aErrorStack )
             == STL_SUCCESS );
    
    while ( 1 )
    {
        STL_TRY( ztqSQLFetch( sStmtHandle,
                              &sResult,
                              aErrorStack )
                 == STL_SUCCESS );

        /**
         * Sequence 는 Privilege 가 한 종류이다
         */
        
        if ( sResult == SQL_NO_DATA )
        {
            ztqBuildPrivOrder( & sPrivOrder, sOwnerID );
            
            STL_TRY( ztqPrintGrantByPrivOrder( aStringDDL,
                                               & sPrivOrder,
                                               ZTQ_PRIV_OBJECT_SEQUENCE,
                                               NULL, /* aPrivName */
                                               sSchemaName, 
                                               sSeqName,
                                               NULL, /* aColumnName */
                                               aErrorStack )
                     == STL_SUCCESS );
            break;
        }

        STL_TRY( ztqAddPrivItem( & sPrivOrder,
                                 sGrantorID,
                                 sGrantorName,
                                 sGranteeID,
                                 sGranteeName,
                                 sIsBuiltIn,
                                 sGrantable,
                                 aAllocator,
                                 aErrorStack )
                 == STL_SUCCESS );
    }

    /******************************
     * 마무리 
     ******************************/

    STL_RAMP( RAMP_TABLE_FINISH );
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /**
     * Memory Mark 해제
     */
    
    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        STL_TRY( stlRestoreRegionMem( aAllocator,
                                      &sMark,
                                      STL_FALSE, /* aFree */
                                      aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    if( sIsMarked == STL_TRUE )
    {
        sIsMarked = STL_FALSE;
        (void) stlRestoreRegionMem( aAllocator,
                                    &sMark,
                                    STL_FALSE, /* aFree */
                                    aErrorStack );
    }
    
    return STL_FAILURE;
}



stlStatus ztqPrintCommentSequence( stlInt64        aSeqID,
                                   stlChar       * aStringDDL,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sSeqName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSeqInd;
    stlChar     sComment[ZTQ_MAX_COMMENT_LENGTH];
    SQLLEN      sCommentInd;


    /************************************************************
     * COMMENT 꽌련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME  "
                 "      , sch.SCHEMA_NAME          "
                 "      , seq.SEQUENCE_NAME        "
                 "      , seq.COMMENTS             "
                 "   FROM "
                 "        DEFINITION_SCHEMA.SEQUENCES             AS seq "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS        AS auth "
                 "  WHERE "
                 "        seq.SCHEMA_ID   = sch.SCHEMA_ID "
                 "    AND seq.OWNER_ID    = auth.AUTH_ID "
                 "    AND seq.SEQUENCE_ID = %ld ",
                 aSeqID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sSeqName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSeqInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sComment,
                            ZTQ_MAX_COMMENT_LENGTH,
                            & sCommentInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * COMMENT 구문 생성
     ************************************************************/

    if ( sCommentInd == SQL_NULL_DATA )
    {
        /* comment 가 없는 경우 */
    }
    else
    {
        /**
         * SET SESSION AUTHORIZATION owner
         */
    
        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "\nSET SESSION AUTHORIZATION \"%s\"; ",
                     sOwnerName );
    
        /**
         * COMMENT
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\nCOMMENT "
                     "\n    ON SEQUENCE \"%s\".\"%s\" ",
                     aStringDDL,
                     sSchemaName,
                     sSeqName );

        /**
         * IS 'comment'
         */

        stlSnprintf( aStringDDL,
                     gZtqDdlSize,
                     "%s"
                     "\n    IS '%s' ",
                     aStringDDL,
                     sComment );
    
        STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


/****************************************************************************
 * Synonym
 ****************************************************************************/

stlStatus ztqGetSynonymID( ztqIdentifier * aIdentifierList,
                           stlAllocator  * aAllocator,
                           stlInt64      * aSynonymID,
                           stlErrorStack * aErrorStack )
{
    stlChar  * sIdentifier = NULL;
    stlInt32   sLen = 0;

    stlBool    sIsQualified = STL_FALSE;
    stlChar    sSynonymName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar    sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    /************************************************************
     * double-quoted identifier 처리 
     ************************************************************/

    if ( aIdentifierList->mNext == NULL )
    {
        /**
         * synonym_name 만 기술한 경우 
         */

        sIsQualified = STL_FALSE;
        sIdentifier = aIdentifierList->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sSynonymName, sIdentifier + 1, sLen - 2 );
            sSynonymName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sSynonymName, sIdentifier );
        }
    }
    else
    {
        /**
         * schema_name.index_name 으로  기술한 경우 
         */
        
        sIsQualified = STL_TRUE;

        sIdentifier = aIdentifierList->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sSchemaName, sIdentifier + 1, sLen - 2 );
            sSchemaName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sSchemaName, sIdentifier );
        }

        sIdentifier = ((ztqIdentifier *) aIdentifierList->mNext)->mIdentifier;
        
        if ( sIdentifier[0] == '"' )
        {
            sLen = stlStrlen( sIdentifier );
            stlMemcpy( sSynonymName, sIdentifier + 1, sLen - 2 );
            sSynonymName[sLen-2] = '\0';
        }
        else
        {
            stlStrcpy( sSynonymName, sIdentifier );
        }
    }

    /************************************************************
     * Synonym ID 획득
     ************************************************************/

    if ( sIsQualified == STL_TRUE )
    {
        STL_TRY( ztqGetQualifiedSynonymID( sSchemaName,
                                           sSynonymName,
                                           aAllocator,
                                           aSynonymID,
                                           aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztqGetNonQualifiedSynonymID( sSynonymName,
                                              aAllocator,
                                              aSynonymID,
                                              aErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqGetQualifiedSynonymID( stlChar       * aSchemaName,
                                    stlChar       * aSynonymName,
                                    stlAllocator  * aAllocator,
                                    stlInt64      * aSynonymID,
                                    stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSynonymID;
    SQLLEN      sSynonymInd;
    
    /************************************************************
     * SYNONYM ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT syn.SYNONYM_ID "
                 "  FROM DEFINITION_SCHEMA.SYNONYMS syn "
                 "     , DEFINITION_SCHEMA.SCHEMATA sch "
                 " WHERE syn.SCHEMA_ID = sch.SCHEMA_ID "
                 "   AND syn.SYNONYM_NAME = '%s' "
                 "   AND sch.SCHEMA_NAME = '%s' ",
                 aSynonymName,
                 aSchemaName );

    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSynonymID,
                            STL_SIZEOF(sSynonymID),
                            & sSynonymInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aSynonymID = sSynonymID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqGetNonQualifiedSynonymID( stlChar       * aSynonymName,
                                       stlAllocator  * aAllocator,
                                       stlInt64      * aSynonymID,
                                       stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    
    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;

    stlInt64    sSynonymID;
    SQLLEN      sSynonymInd;
    
    /************************************************************
     * SEQUENCE ID 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        syn.SYNONYM_ID "
                 "   FROM "
                 "        DEFINITION_SCHEMA.SCHEMATA         AS sch "
                 "      , DEFINITION_SCHEMA.SYNONYMS         AS syn "
                 "      , ( "
                 "         ( SELECT "
                 "                sch1.SCHEMA_ID  "
                 "              , path1.SEARCH_ORDER  "
                 "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path1 "
                 "              , DEFINITION_SCHEMA.SCHEMATA         AS sch1 "
                 "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr1 "
                 "          WHERE "
                 "                usr1.AUTHORIZATION_NAME = CURRENT_USER "
                 "            AND path1.SCHEMA_ID         = sch1.SCHEMA_ID "
                 "            AND path1.AUTH_ID           = usr1.AUTH_ID "
                 "          ORDER BY "
                 "                path1.SEARCH_ORDER "
                 "         ) "
                 "       UNION ALL "
                 "         ( SELECT "
                 "                sch2.SCHEMA_ID "
                 "              , ( SELECT COUNT(*) "
                 "                    FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH "
                 "                   WHERE AUTH_ID = USER_ID() ) "
                 "                + path2.SEARCH_ORDER "
                 "           FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH AS path2 "
                 "              , DEFINITION_SCHEMA.SCHEMATA         AS sch2  "
                 "              , DEFINITION_SCHEMA.AUTHORIZATIONS   AS usr2 "
                 "          WHERE "
                 "                usr2.AUTHORIZATION_NAME = 'PUBLIC' "
                 "            AND path2.SCHEMA_ID         = sch2.SCHEMA_ID "
                 "            AND path2.AUTH_ID           = usr2.AUTH_ID "
                 "          ORDER BY "
                 "                path2.SEARCH_ORDER "
                 "         ) "
                 "       ) AS upath ( SCHEMA_ID, SEARCH_ORDER ) "
                 " WHERE syn.SCHEMA_ID = sch.SCHEMA_ID "
                 "   AND sch.SCHEMA_ID = upath.SCHEMA_ID "
                 "   AND syn.SYNONYM_NAME = '%s' "
                 " ORDER BY "
                 "       upath.SEARCH_ORDER  "
                 " FETCH 1 ",
                 aSynonymName );
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SBIGINT,
                            & sSynonymID,
                            STL_SIZEOF(sSynonymID),
                            & sSynonymInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    
    /************************************************************
     * Output 설정
     ************************************************************/

    *aSynonymID = sSynonymID;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqPrintCreateSynonym( stlInt64        aSynonymID,
                                 stlChar       * aStringDDL,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];

    SQLHSTMT    sStmtHandle = NULL;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlChar     sOwnerName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sOwnerInd;
    stlChar     sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSchemaInd;
    stlChar     sSynonymName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sSynonymInd;

    stlChar     sObjectSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sObjectSchemaNameInd;
    stlChar     sObjectName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLLEN      sObjectNameInd;
    
    /************************************************************
     * CREATE SYNONYM 관련 정보 획득
     ************************************************************/

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 " SELECT "
                 "        auth.AUTHORIZATION_NAME  "
                 "      , sch.SCHEMA_NAME          "
                 "      , syn.SYNONYM_NAME         "
                 "      , syn.OBJECT_SCHEMA_NAME   "
                 "      , syn.OBJECT_NAME          "
                 "   FROM "
                 "        DEFINITION_SCHEMA.SYNONYMS              AS syn "
                 "      , DEFINITION_SCHEMA.SCHEMATA              AS sch  "
                 "      , DEFINITION_SCHEMA.AUTHORIZATIONS        AS auth "
                 "  WHERE "
                 "        syn.SCHEMA_ID   = sch.SCHEMA_ID "
                 "    AND syn.OWNER_ID    = auth.AUTH_ID "
                 "    AND syn.SYNONYM_ID = %ld ",
                 aSynonymID );
    
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*) sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            sOwnerName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sOwnerInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            2,
                            SQL_C_CHAR,
                            sSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSchemaInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            3,
                            SQL_C_CHAR,
                            sSynonymName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sSynonymInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            4,
                            SQL_C_CHAR,
                            sObjectSchemaName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sObjectSchemaNameInd,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            5,
                            SQL_C_CHAR,
                            sObjectName,
                            STL_MAX_SQL_IDENTIFIER_LENGTH,
                            & sObjectNameInd,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );

    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    /************************************************************
     * CREATE SYNONYM 구문 생성
     ************************************************************/

    /**
     * SET SESSION AUTHORIZATION grantor
     */
    
    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "\nSET SESSION AUTHORIZATION \"%s\"; ",
                 sOwnerName );
    
    /**
     * CREATE SYNONYM
     */

    stlSnprintf( aStringDDL,
                 gZtqDdlSize,
                 "%s"
                 "\nCREATE SYNONYM \"%s\".\"%s\" FOR \"%s\".\"%s\"",
                 aStringDDL,
                 sSchemaName,
                 sSynonymName,
                 sObjectSchemaName,
                 sObjectName );
    
    
    STL_TRY( ztqFinishPrintDDL( aStringDDL, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

/** @} */
