/*******************************************************************************
 * smlKey.c
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

#include <stl.h>
#include <knl.h>
#include <scl.h>
#include <dtl.h>
#include <sml.h>
#include <smDef.h>
#include <sme.h>
#include <smn.h>
#include <sms.h>

/**
 * @file smlKey.c
 * @brief Storage Manager Layer Index Key Routines
 */

/**
 * @addtogroup smlKey
 * @{
 */


/**
 * @brief 주어진 Index에 새 Key를 삽입한다
 * @param[in] aStatement 사용중인 statement
 * @param[in] aRelationHandle Key를 삽입할 Index의 Relation의 handle
 * @param[in] aColumns 삽입할 Key의 컬럼 정보들
 * @param[in] aRowBlock SM Row Block
 * @param[in] aBlockFilter Block내에 삽입해야할 row에 대한 boolean형 filter (nullable)
 * @param[in] aIsDeferred Unique violation을 Deferred로 처리할지 여부
 * @param[out] aViolationCnt uniqueness violation된 개수
 * @param[out] aUniqueViolationScope 삽입할 key가 uniqueness violation을 발생시킨 범위
 * @param[in] aEnv Storage Manager Environment
 * @see @a aUniqueViolationScope : smlUniqueViolationScope
 */
stlStatus smlInsertKeys( smlStatement            * aStatement,
                         void                    * aRelationHandle,
                         knlValueBlockList       * aColumns,
                         smlRowBlock             * aRowBlock,
                         stlBool                 * aBlockFilter,
                         stlBool                   aIsDeferred,
                         stlInt32                * aViolationCnt,
                         smlUniqueViolationScope * aUniqueViolationScope,
                         smlEnv                  * aEnv )
{
    smlRelationType           sRelType = SME_GET_RELATION_TYPE( aRelationHandle );
    smnIndexModule          * sModule = smnGetIndexModule(SML_GET_INDEX_TYPE(sRelType));
    stlInt32                  sUsedBlockCnt;
    stlInt32                  sSkipBlockCnt;
    smlRid                    sRowRid;
    smlUniqueViolationScope   sUniqueViolationScope;
    smlRid                    sUndoRid = SML_INVALID_RID;
    smlTbsId                  sTbsId;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    /**
     * Validate tablespace
     */

    sTbsId = smsGetTbsId( SME_GET_SEGMENT_HANDLE(aRelationHandle) );
    STL_TRY( smlValidateTablespace( sTbsId,
                                    SME_VALIDATION_BEHAVIOR_UPDATE,
                                    aEnv )
             == STL_SUCCESS );

    sUsedBlockCnt = KNL_GET_BLOCK_USED_CNT( aColumns );
    sSkipBlockCnt = KNL_GET_BLOCK_SKIP_CNT( aColumns );

    *aUniqueViolationScope = SML_UNIQUE_VIOLATION_SCOPE_NONE;

    /**
     * Validate Unusable index
     */

    STL_TRY_THROW( smsIsUsableSegment( SME_GET_SEGMENT_HANDLE(aRelationHandle) )
                   == STL_TRUE, RAMP_FINISH );
    
    STL_TRY_THROW( sUsedBlockCnt > sSkipBlockCnt, RAMP_FINISH );

    if( sUsedBlockCnt == (sSkipBlockCnt + 1) )
    {
        if ( (aBlockFilter == NULL) || ( aBlockFilter[sSkipBlockCnt] == STL_TRUE ) )
        {
            /**
             * Single block의 rollback은 undo key record를 기반으로 한다.
             */
            
            sRowRid = SML_GET_RID_VALUE( aRowBlock, sSkipBlockCnt );
            
            STL_TRY( sModule->mInsert( aStatement,
                                       aRelationHandle,
                                       aColumns,
                                       sSkipBlockCnt,
                                       &sRowRid,
                                       &sUndoRid,
                                       STL_FALSE,    /* aIsIndexBuilding */
                                       aIsDeferred,  /* aIsDeferred */
                                       &sUniqueViolationScope,
                                       aEnv )
                     == STL_SUCCESS );

            *aUniqueViolationScope = sUniqueViolationScope;
            
            if( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_NONE )
            {
                (*aViolationCnt)++;
                STL_TRY_THROW( aIsDeferred == STL_TRUE, RAMP_FINISH );
            }
        }
        else
        {
            /**
             * 조건에 맞는 Row 가 아님
             */
            
            *aUniqueViolationScope = SML_UNIQUE_VIOLATION_SCOPE_NONE;
        }
    }
    else
    {
        STL_TRY( sModule->mBlockInsert( aStatement,
                                        aRelationHandle,
                                        aColumns,
                                        aRowBlock,
                                        aBlockFilter,
                                        aIsDeferred,  /* aIsDeferred */
                                        aViolationCnt,
                                        aUniqueViolationScope,
                                        aEnv )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_FINISH );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}


/**
 * @brief 주어진 Index에서 Key를 삭제한다
 * @param[in] aStatement 사용중인 statement
 * @param[in] aRelationHandle Key를 삭제할 Index의 Relation의 handle
 * @param[in] aColumns 삭제할 Key의 컬럼 정보들
 * @param[in] aRowBlock SM Row Block
 * @param[in] aBlockFilter Block내에 삽입해야할 row에 대한 boolean형 filter (nullable)
 * @param[out] aViolationCnt uniqueness violation된 개수
 * @param[out] aUniqueViolationScope 삭제할 key가 uniqueness violation을 해소시키는지 여부
 * @param[in] aEnv Storage Manager Environment
 * @see @a aUniqueViolationScope : smlUniqueViolationScope
 */
stlStatus smlDeleteKeys( smlStatement            * aStatement,
                         void                    * aRelationHandle,
                         knlValueBlockList       * aColumns,
                         smlRowBlock             * aRowBlock,
                         stlBool                 * aBlockFilter,
                         stlInt32                * aViolationCnt,
                         smlUniqueViolationScope * aUniqueViolationScope,
                         smlEnv                  * aEnv )
{
    smlRelationType           sRelType = SME_GET_RELATION_TYPE( aRelationHandle );
    smnIndexModule          * sModule = smnGetIndexModule(SML_GET_INDEX_TYPE(sRelType));
    stlInt32                  sUsedBlockCnt;
    stlInt32                  sSkipBlockCnt;
    smlRid                    sRowRid;
    smlUniqueViolationScope   sUniqueViolationScope;
    smlRid                    sUndoRid = SML_INVALID_RID;
    smlTbsId                  sTbsId;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    /**
     * Validate relation
     */

    sTbsId = smsGetTbsId( SME_GET_SEGMENT_HANDLE(aRelationHandle) );
    STL_TRY( smlValidateTablespace( sTbsId,
                                    SME_VALIDATION_BEHAVIOR_UPDATE,
                                    aEnv )
             == STL_SUCCESS );

    sUsedBlockCnt = KNL_GET_BLOCK_USED_CNT( aColumns );
    sSkipBlockCnt = KNL_GET_BLOCK_SKIP_CNT( aColumns );

    *aUniqueViolationScope = SML_UNIQUE_VIOLATION_SCOPE_NONE;

    /**
     * Validate Unusable index
     */

    STL_TRY_THROW( smsIsUsableSegment( SME_GET_SEGMENT_HANDLE(aRelationHandle)  )
                   == STL_TRUE, RAMP_FINISH );
    
    STL_TRY_THROW( sUsedBlockCnt > sSkipBlockCnt, RAMP_FINISH );

    if( sUsedBlockCnt == (sSkipBlockCnt + 1) )
    {
        if ( (aBlockFilter == NULL) || ( aBlockFilter[sSkipBlockCnt] == STL_TRUE ) )
        {
            /**
             * Single block의 rollback은 undo key record를 기반으로 한다.
             */
            
            sRowRid = SML_GET_RID_VALUE( aRowBlock, sSkipBlockCnt );
            
            STL_TRY( sModule->mDelete( aStatement,
                                       aRelationHandle,
                                       aColumns,
                                       sSkipBlockCnt,
                                       &sRowRid,
                                       &sUndoRid,
                                       &sUniqueViolationScope,
                                       aEnv ) == STL_SUCCESS );
            
            if( sUniqueViolationScope == SML_UNIQUE_VIOLATION_SCOPE_RESOLVED )
            {
                (*aViolationCnt)--;
            }
            
            *aUniqueViolationScope = sUniqueViolationScope;
        }
        else
        {
            /**
             * 조건에 맞는 Row 가 아님
             */
            
            *aUniqueViolationScope = SML_UNIQUE_VIOLATION_SCOPE_NONE;
        }
    }
    else
    {
        STL_TRY( sModule->mBlockDelete( aStatement,
                                        aRelationHandle,
                                        aColumns,
                                        aRowBlock,
                                        aBlockFilter,
                                        aViolationCnt,
                                        aUniqueViolationScope,
                                        aEnv )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_FINISH );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}



/** @} */
