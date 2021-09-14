/*******************************************************************************
 * smxmBlock.c
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
#include <smxmDef.h>
#include <smxm.h>

/**
 * @file smxmBlock.c
 * @brief Storage Manager Layer Mini-Transaction Log Block Internal Routines
 */

/**
 * @addtogroup smxmBlock
 * @{
 */

/**
 * @brief Mini-transaction Log Block으로부터 aLogBuffer에 aReadSize만큼 Log Data를 복사한다.
 * @param[in] aLogBlockCursor Log Block Scan을 위한 Cursor
 * @param[in] aLogBlock Log Block
 * @param[in] aLogBuffer 복사될 Log Buffer
 * @param[in] aReadSize 복사할 Log Data 크기
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smxmCopyLogFromBlock( smxmLogBlockCursor * aLogBlockCursor,
                                void               * aLogBlock,
                                void               * aLogBuffer,
                                stlUInt32            aReadSize,
                                smlEnv             * aEnv )
{
    smxmLogBlockSlot * sLogBlockSlot;
    smxmLogBlock     * sLogBlock;
    stlUInt32          sReadSize = 0;
    stlUInt32          sTotalReadSize = 0;

    sLogBlock = (smxmLogBlock*)aLogBlock;
    
    while( sTotalReadSize < aReadSize )
    {
        sLogBlockSlot = &sLogBlock->mLogBlockSlotArray[aLogBlockCursor->mCurBlockIdx];
        STL_ASSERT( sLogBlockSlot->mBuffer != NULL );

        if( aLogBlockCursor->mCurOffset < sLogBlockSlot->mSize )
        {
            sReadSize = sLogBlockSlot->mSize - aLogBlockCursor->mCurOffset;
            if( (aReadSize - sTotalReadSize) < sReadSize )
            {
                sReadSize = aReadSize - sTotalReadSize;
            }

            stlMemcpy( aLogBuffer + sTotalReadSize,
                       sLogBlockSlot->mBuffer + aLogBlockCursor->mCurOffset,
                       sReadSize );

            aLogBlockCursor->mCurOffset += sReadSize;
            sTotalReadSize += sReadSize;
        }
        else
        {
            aLogBlockCursor->mCurOffset = 0;
            aLogBlockCursor->mCurBlockIdx++;
        }
    }
    
    return STL_SUCCESS;
}
                       
/** @} */

/**
 * @addtogroup smxmBlock
 * @{
 */


/** @} */
