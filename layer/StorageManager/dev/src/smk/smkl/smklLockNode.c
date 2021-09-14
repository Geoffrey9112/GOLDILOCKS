/*******************************************************************************
 * smklLockNode.c
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
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smxl.h>
#include <sme.h>
#include <smg.h>
#include <smklDef.h>
#include <smklLockNode.h>
#include <smklWaitGraph.h>
#include <smklManager.h>

/**
 * @file smklLockNode.c
 * @brief Storage Manager Layer Lock Node Internal Routines
 */

extern stlChar  gLockConversionMatrix[6][6];
extern stlChar  gLockCompatibleMatrix[6][6];

/**
 * @addtogroup smklLockNode
 * @{
 */

smklNode * smklFindLockNode( smxlTransId    aTransId,
                             smklItemCore * aLockItem )
{
    smklNode * sLockNode;
    smklNode * sTargetLockNode = NULL;
    
    STL_RING_FOREACH_ENTRY( &aLockItem->mHoldList, sLockNode )
    {
        if( SMXL_TRANS_SLOT_ID(sLockNode->mTransId) == SMXL_TRANS_SLOT_ID(aTransId) )
        {
            sTargetLockNode = sLockNode;
            break;
        }
    }

    return sTargetLockNode;
}

stlStatus smklRemoveLockNode( smklNode * aLockNode )
{
    smklItemCore  * sLockItem;
    smklNode      * sLockNode;
    stlUInt32       sLockMode;

    sLockItem = aLockNode->mLockItem;

    STL_RING_UNLINK( &sLockItem->mHoldList, aLockNode );

    sLockMode = SML_LOCK_NONE;
    
    STL_RING_FOREACH_ENTRY( &sLockItem->mHoldList, sLockNode )
    {
        sLockMode = gLockConversionMatrix[sLockMode][sLockNode->mLockMode];
    }

    sLockItem->mGrantedMode = sLockMode;

    STL_DASSERT( smklValidateLockItem( sLockItem ) == STL_TRUE );
    
    return STL_SUCCESS;
}

stlStatus smklCheckLockConflict( smklItemCore * aLockItem,
                                 smklNode     * aLockNode,
                                 stlUInt32      aLockMode,
                                 smlEnv       * aEnv )
{
    smklNode   * sLockNode;
    stlUInt32    sLockMode;

    sLockMode = SML_LOCK_NONE;

    if( aLockNode == NULL )
    {
        sLockMode = aLockItem->mGrantedMode;
    }
    else
    {
        STL_RING_FOREACH_ENTRY( &aLockItem->mHoldList, sLockNode )
        {
            if( sLockNode != aLockNode )
            {
                sLockMode = gLockConversionMatrix[sLockMode][sLockNode->mLockMode];
            }
        }
    }
    
    STL_TRY_THROW( gLockCompatibleMatrix[sLockMode][aLockMode] == STL_TRUE,
                   RAMP_ERR_LOCK_CONFLICT );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_LOCK_CONFLICT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_LOCK_TIMEOUT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smklAddLockNode( smxlTransId     aTransId,
                           smklLockSlot  * aLockSlot,
                           smklNode      * aHoldLockNode,
                           stlUInt32       aLockMode,
                           smklNode     ** aReqLockNode,
                           stlBool       * aWaitFlag,
                           smlEnv        * aEnv )
{
    smklItemCore  * sLockItem;
    smklNode      * sLockNode;
    stlUInt32       sGrantedMode;

    sLockItem = aHoldLockNode->mLockItem;
    sGrantedMode = SML_LOCK_NONE;

    STL_RING_FOREACH_ENTRY( &sLockItem->mHoldList, sLockNode )
    {
        if( SMXL_TRANS_SLOT_ID(sLockNode->mTransId) == SMXL_TRANS_SLOT_ID(aTransId) )
        {
            continue;
        }

        sGrantedMode = gLockConversionMatrix[sLockNode->mLockMode][sGrantedMode];
    }
    
    if( gLockCompatibleMatrix[sGrantedMode][aLockMode] == STL_TRUE )
    {
        aHoldLockNode->mLockMode = gLockConversionMatrix[aHoldLockNode->mLockMode][aLockMode];
        sLockItem->mGrantedMode = gLockConversionMatrix[sGrantedMode][aHoldLockNode->mLockMode];

        *aWaitFlag = STL_FALSE;
        *aReqLockNode = aHoldLockNode;
    }
    else
    {
        STL_RING_FOREACH_ENTRY( &sLockItem->mHoldList, sLockNode )
        {
            if( SMXL_TRANS_SLOT_ID(sLockNode->mTransId) == SMXL_TRANS_SLOT_ID(aTransId) )
            {
                continue;
            }
            
            SMKL_ADD_EDGE( smklGetLockSlotId( aHoldLockNode->mTransId ),
                           smklGetLockSlotId( sLockNode->mTransId ) );
        }

        STL_TRY( smxlAllocShmMem( aTransId,
                                  STL_SIZEOF( smklNode ),
                                  (void**)&sLockNode,
                                  aEnv ) == STL_SUCCESS );
                
        STL_RING_INIT_DATALINK( sLockNode,
                                STL_OFFSETOF( smklNode, mTransLink ) );
        STL_RING_INIT_DATALINK( sLockNode,
                                STL_OFFSETOF( smklNode, mNodeLink ) );
    
        sLockNode->mLockItem = sLockItem;
        sLockNode->mLockMode = aLockMode;
        sLockNode->mTransId = aTransId;
    
        STL_RING_ADD_LAST( &aLockSlot->mLockNodeList, sLockNode );
        STL_RING_ADD_LAST( &sLockItem->mRequestList, sLockNode );
        
        sLockNode->mEnv = aEnv;
        
        *aWaitFlag = STL_TRUE;
        *aReqLockNode = sLockNode;
    }
    
    STL_DASSERT( smklValidateLockItem( sLockItem ) == STL_TRUE );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smklAddNewLockNode( smxlTransId    aTransId,
                              smklLockSlot * aLockSlot,
                              smklItemCore * aLockItem,
                              stlUInt32      aLockMode,
                              smklNode    ** aLockNode,
                              stlBool      * aWaitFlag,
                              smlEnv       * aEnv )
{
    smklNode  * sLockNode;
    smklNode  * sHoldLockNode;
    stlUInt32   sLockMode;
    stlUInt32   sGrantedMode;

    STL_TRY( smxlAllocShmMem( aTransId,
                              STL_SIZEOF( smklNode ),
                              (void**)&sLockNode,
                              aEnv ) == STL_SUCCESS );
                
    STL_RING_INIT_DATALINK( sLockNode,
                            STL_OFFSETOF( smklNode, mTransLink ) );
    STL_RING_INIT_DATALINK( sLockNode,
                            STL_OFFSETOF( smklNode, mNodeLink ) );
    
    sLockNode->mLockItem = aLockItem;
    sLockNode->mLockMode = aLockMode;
    sLockNode->mTransId = aTransId;
    
    STL_RING_ADD_LAST( &aLockSlot->mLockNodeList, sLockNode );

    sLockMode = sLockNode->mLockMode;
    sGrantedMode = aLockItem->mGrantedMode;

    if( gLockCompatibleMatrix[sGrantedMode][sLockMode] == STL_TRUE )
    {
        STL_RING_ADD_LAST( &aLockItem->mHoldList, sLockNode );
        sGrantedMode = gLockConversionMatrix[sGrantedMode][sLockMode];
        *aWaitFlag = STL_FALSE;
    }
    else
    {
        STL_RING_FOREACH_ENTRY( &aLockItem->mHoldList, sHoldLockNode )
        {
            SMKL_ADD_EDGE( smklGetLockSlotId( sLockNode->mTransId ),
                           smklGetLockSlotId( sHoldLockNode->mTransId ) );
        }
        
        STL_RING_ADD_LAST( &aLockItem->mRequestList, sLockNode );
        sLockNode->mEnv = aEnv;
        *aWaitFlag = STL_TRUE;
    }
    
    *aLockNode = sLockNode;
    aLockItem->mGrantedMode = sGrantedMode;
    
    STL_DASSERT( smklValidateLockItem( aLockItem ) == STL_TRUE );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
