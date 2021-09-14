/*******************************************************************************
 * smnmpbCompFuncs.c
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
#include <sml.h>
#include <smDef.h>
#include <sms.h>
#include <sme.h>
#include <smp.h>
#include <smg.h>
#include <sma.h>
#include <smd.h>
#include <smxm.h>
#include <smxl.h>
#include <smnDef.h>
#include <smnmpbDef.h>
#include <smnmpb.h>


/**
 * @file smnmpbCompFuncs.c
 * @brief Storage Manager Layer Memory B-Tree Index Build Compare Routines
 */

/**
 * @addtogroup smn
 * @{
 */

stlInt32 smnmpbCompare1SortNonPrimary( void * aCompareInfo,
                                       void * aValueA,
                                       void * aValueB )
{
    smnmpbCompareInfo * sCompareInfo = (smnmpbCompareInfo*)aCompareInfo;
    stlChar           * sKeyA;
    stlChar           * sKeyB;
    stlChar           * sKeyHdrA;
    stlChar           * sKeyHdrB;
    stlUInt8            sColLenSizeA = 1;
    stlInt64            sColLenA;
    stlUInt8            sColLenSizeB = 1;
    stlInt64            sColLenB;
    dtlCompareResult    sResult = DTL_COMPARISON_EQUAL;
    stlInt8           * sKeyOrder;
    stlUInt8          * sKeyFlags;
    smlPid              sPageIdA;
    smlPid              sPageIdB;
    smpOffsetSlot       sOffsetA;
    smpOffsetSlot       sOffsetB;
    stlInt32            i = 0;
    stlInt32            sNullColCnt = 0;
    dtlCompArg          sCompArg;

    sKeyOrder = sCompareInfo->mKeyColOrder;
    sKeyFlags = sCompareInfo->mKeyColFlags;

    sKeyHdrA = sCompareInfo->mSortBlock + *((smnmpbSortOffset*)aValueA);
    sKeyHdrB = sCompareInfo->mSortBlock + *((smnmpbSortOffset*)aValueB);

    sKeyA = sKeyHdrA + SMNMPB_RUN_KEY_HDR_LEN;
    sKeyB = sKeyHdrB + SMNMPB_RUN_KEY_HDR_LEN;

    do
    {
        sColLenA = *(stlUInt8*)sKeyA;
        sColLenB = *(stlUInt8*)sKeyB;
        
        if( sColLenA == 0 )
        {
            sNullColCnt++;
            
            /**
             * value1이 NULL
             */
            if( sColLenB == 0 )
            {
                /* value1(NULL) = value2(NULL) */
                sResult = DTL_COMPARISON_EQUAL;
            }
            else if( (sKeyFlags[i] & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                     == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
            {
                /* value1(NULL) < value2(NOTNULL) */
                sResult = DTL_COMPARISON_LESS;
                break;
            }
            else 
            {
                /* value1(NULL) > value2(NOTNULL) */
                sResult = DTL_COMPARISON_GREATER;
                break;
            }
        }
        else
        {
            /**
             * value1이 NOT NULL
             */
            if( sColLenB == 0 )
            {
                if( (sKeyFlags[i] & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                    == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
                {                    
                    /* value1(NOTNULL) > value2(NULL) */
                    sResult = DTL_COMPARISON_GREATER;
                    break;
                }
                else 
                {
                    /* value1(NOTNULL) < value2(NULL) */
                    sResult = DTL_COMPARISON_LESS;
                    break;
                }
            }
            else 
            {
                /**
                 * value1(NOTNULL) & value2(NOTNULL)
                 */

                DTL_TYPED_COMPARE( sCompareInfo->mPhysicalCompare[i],
                                   sResult,
                                   sCompArg,
                                   sKeyA + sColLenSizeA,
                                   sColLenA,
                                   sKeyB + sColLenSizeB,
                                   sColLenB );

                if( sResult != DTL_COMPARISON_EQUAL )
                {
                    /**
                     * Key Ordering 보정
                     */
                    sResult *= sKeyOrder[i];
                    break;
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }

        i++;
        
        if( i >= sCompareInfo->mKeyColCount )
        {
            sCompareInfo->mUniqueViolated += ( sCompareInfo->mKeyColCount != sNullColCnt );
        
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrA, &sPageIdA );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrA, &sOffsetA );
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrB, &sPageIdB );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrB, &sOffsetB );
                
            if( sPageIdA > sPageIdB )
            {
                sResult = DTL_COMPARISON_GREATER;
            }
            else if( sPageIdA < sPageIdB )
            {
                sResult = DTL_COMPARISON_LESS;
            }
            else
            {
                if( sOffsetA > sOffsetB )
                {
                    sResult = DTL_COMPARISON_GREATER;
                }
                else if( sOffsetA < sOffsetB )
                {
                    sResult = DTL_COMPARISON_LESS;
                }
                else
                {
                    sResult = DTL_COMPARISON_EQUAL;
                    sCompareInfo->mUniqueViolated -= ( sCompareInfo->mKeyColCount != sNullColCnt );
                }
            }
            
            break;
        }

        sKeyA += (sColLenSizeA + sColLenA);
        sKeyB += (sColLenSizeB + sColLenB);

    } while( 1 );

    return sResult;
}

stlInt32 smnmpbCompare1SortPrimary( void * aCompareInfo,
                                    void * aValueA,
                                    void * aValueB )
{
    smnmpbCompareInfo * sCompareInfo = (smnmpbCompareInfo*)aCompareInfo;
    stlChar           * sKeyA;
    stlChar           * sKeyB;
    stlChar           * sKeyHdrA;
    stlChar           * sKeyHdrB;
    stlUInt8            sColLenSizeA = 1;
    stlInt64            sColLenA;
    stlUInt8            sColLenSizeB = 1;
    stlInt64            sColLenB;
    dtlCompareResult    sResult = DTL_COMPARISON_EQUAL;
    stlInt8           * sKeyOrder;
    smlPid              sPageIdA;
    smlPid              sPageIdB;
    smpOffsetSlot       sOffsetA;
    smpOffsetSlot       sOffsetB;
    stlInt32            i = 0;
    dtlCompArg          sCompArg;

    sKeyOrder = sCompareInfo->mKeyColOrder;

    sKeyHdrA = sCompareInfo->mSortBlock + *((smnmpbSortOffset*)aValueA);
    sKeyHdrB = sCompareInfo->mSortBlock + *((smnmpbSortOffset*)aValueB);

    sKeyA = sKeyHdrA + SMNMPB_RUN_KEY_HDR_LEN;
    sKeyB = sKeyHdrB + SMNMPB_RUN_KEY_HDR_LEN;

    do
    {
        sColLenA = *(stlUInt8*)sKeyA;
        sColLenB = *(stlUInt8*)sKeyB;
        
        DTL_TYPED_COMPARE( sCompareInfo->mPhysicalCompare[i],
                           sResult,
                           sCompArg,
                           sKeyA + sColLenSizeA,
                           sColLenA,
                           sKeyB + sColLenSizeB,
                           sColLenB );

        if( sResult != DTL_COMPARISON_EQUAL )
        {
            /**
             * Key Ordering 보정
             */
            sResult *= sKeyOrder[i];
            break;
        }
        else
        {
            /* Do Nothing */
        }

        i++;
        
        if( i >= sCompareInfo->mKeyColCount )
        {
            sCompareInfo->mUniqueViolated += 1;
        
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrA, &sPageIdA );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrA, &sOffsetA );
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrB, &sPageIdB );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrB, &sOffsetB );
                
            if( sPageIdA > sPageIdB )
            {
                sResult = DTL_COMPARISON_GREATER;
            }
            else if( sPageIdA < sPageIdB )
            {
                sResult = DTL_COMPARISON_LESS;
            }
            else
            {
                if( sOffsetA > sOffsetB )
                {
                    sResult = DTL_COMPARISON_GREATER;
                }
                else if( sOffsetA < sOffsetB )
                {
                    sResult = DTL_COMPARISON_LESS;
                }
                else
                {
                    sResult = DTL_COMPARISON_EQUAL;
                    sCompareInfo->mUniqueViolated -= 1;
                }
            }
            
            break;
        }

        sKeyA += (sColLenSizeA + sColLenA);
        sKeyB += (sColLenSizeB + sColLenB);

    } while( 1 );

    return sResult;
}

stlInt32 smnmpbCompareNSortNonPrimary( void * aCompareInfo,
                                       void * aValueA,
                                       void * aValueB )
{
    smnmpbCompareInfo * sCompareInfo = (smnmpbCompareInfo*)aCompareInfo;
    stlChar           * sKeyA;
    stlChar           * sKeyB;
    stlChar           * sKeyHdrA;
    stlChar           * sKeyHdrB;
    stlUInt8            sColLenSizeA;
    stlInt64            sColLenA;
    stlUInt8            sColLenSizeB;
    stlInt64            sColLenB;
    dtlCompareResult    sResult = DTL_COMPARISON_EQUAL;
    stlInt8           * sKeyOrder;
    stlUInt8          * sKeyFlags;
    smlPid              sPageIdA;
    smlPid              sPageIdB;
    smpOffsetSlot       sOffsetA;
    smpOffsetSlot       sOffsetB;
    stlInt32            i = 0;
    stlInt32            sNullColCnt = 0;
    dtlCompArg          sCompArg;

    sKeyOrder = sCompareInfo->mKeyColOrder;
    sKeyFlags = sCompareInfo->mKeyColFlags;

    sKeyHdrA = sCompareInfo->mSortBlock + *((smnmpbSortOffset*)aValueA);
    sKeyHdrB = sCompareInfo->mSortBlock + *((smnmpbSortOffset*)aValueB);

    sKeyA = sKeyHdrA + SMNMPB_RUN_KEY_HDR_LEN;
    sKeyB = sKeyHdrB + SMNMPB_RUN_KEY_HDR_LEN;

    do
    {
        SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyA, &sColLenSizeA, &sColLenA );
        SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyB, &sColLenSizeB, &sColLenB );
        
        if( sColLenA == 0 )
        {
            sNullColCnt++;
            
            /**
             * value1이 NULL
             */
            if( sColLenB == 0 )
            {
                /* value1(NULL) = value2(NULL) */
                sResult = DTL_COMPARISON_EQUAL;
            }
            else if( (sKeyFlags[i] & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                     == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
            {
                /* value1(NULL) < value2(NOTNULL) */
                sResult = DTL_COMPARISON_LESS;
                break;
            }
            else 
            {
                /* value1(NULL) > value2(NOTNULL) */
                sResult = DTL_COMPARISON_GREATER;
                break;
            }
        }
        else
        {
            /**
             * value1이 NOT NULL
             */
            if( sColLenB == 0 )
            {
                if( (sKeyFlags[i] & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                    == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
                {                    
                    /* value1(NOTNULL) > value2(NULL) */
                    sResult = DTL_COMPARISON_GREATER;
                    break;
                }
                else 
                {
                    /* value1(NOTNULL) < value2(NULL) */
                    sResult = DTL_COMPARISON_LESS;
                    break;
                }
            }
            else 
            {
                /**
                 * value1(NOTNULL) & value2(NOTNULL)
                 */

                DTL_TYPED_COMPARE( sCompareInfo->mPhysicalCompare[i],
                                   sResult,
                                   sCompArg,
                                   sKeyA + sColLenSizeA,
                                   sColLenA,
                                   sKeyB + sColLenSizeB,
                                   sColLenB );

                if( sResult != DTL_COMPARISON_EQUAL )
                {
                    /**
                     * Key Ordering 보정
                     */
                    sResult *= sKeyOrder[i];
                    break;
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }

        i++;
        
        if( i >= sCompareInfo->mKeyColCount )
        {
            sCompareInfo->mUniqueViolated += ( sCompareInfo->mKeyColCount != sNullColCnt );
        
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrA, &sPageIdA );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrA, &sOffsetA );
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrB, &sPageIdB );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrB, &sOffsetB );
                
            if( sPageIdA > sPageIdB )
            {
                sResult = DTL_COMPARISON_GREATER;
            }
            else if( sPageIdA < sPageIdB )
            {
                sResult = DTL_COMPARISON_LESS;
            }
            else
            {
                if( sOffsetA > sOffsetB )
                {
                    sResult = DTL_COMPARISON_GREATER;
                }
                else if( sOffsetA < sOffsetB )
                {
                    sResult = DTL_COMPARISON_LESS;
                }
                else
                {
                    sResult = DTL_COMPARISON_EQUAL;
                    sCompareInfo->mUniqueViolated -= ( sCompareInfo->mKeyColCount != sNullColCnt );
                }
            }
            
            break;
        }

        sKeyA += (sColLenSizeA + sColLenA);
        sKeyB += (sColLenSizeB + sColLenB);

    } while( 1 );

    return sResult;
}

stlInt32 smnmpbCompareNSortPrimary( void * aCompareInfo,
                                    void * aValueA,
                                    void * aValueB )
{
    smnmpbCompareInfo * sCompareInfo = (smnmpbCompareInfo*)aCompareInfo;
    stlChar           * sKeyA;
    stlChar           * sKeyB;
    stlChar           * sKeyHdrA;
    stlChar           * sKeyHdrB;
    stlUInt8            sColLenSizeA;
    stlInt64            sColLenA;
    stlUInt8            sColLenSizeB;
    stlInt64            sColLenB;
    dtlCompareResult    sResult = DTL_COMPARISON_EQUAL;
    stlInt8           * sKeyOrder;
    smlPid              sPageIdA;
    smlPid              sPageIdB;
    smpOffsetSlot       sOffsetA;
    smpOffsetSlot       sOffsetB;
    stlInt32            i = 0;
    dtlCompArg          sCompArg;

    sKeyOrder = sCompareInfo->mKeyColOrder;

    sKeyHdrA = sCompareInfo->mSortBlock + *((smnmpbSortOffset*)aValueA);
    sKeyHdrB = sCompareInfo->mSortBlock + *((smnmpbSortOffset*)aValueB);

    sKeyA = sKeyHdrA + SMNMPB_RUN_KEY_HDR_LEN;
    sKeyB = sKeyHdrB + SMNMPB_RUN_KEY_HDR_LEN;

    do
    {
        SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyA, &sColLenSizeA, &sColLenA );
        SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyB, &sColLenSizeB, &sColLenB );
        
        DTL_TYPED_COMPARE( sCompareInfo->mPhysicalCompare[i],
                           sResult,
                           sCompArg,
                           sKeyA + sColLenSizeA,
                           sColLenA,
                           sKeyB + sColLenSizeB,
                           sColLenB );

        if( sResult != DTL_COMPARISON_EQUAL )
        {
            /**
             * Key Ordering 보정
             */
            sResult *= sKeyOrder[i];
            break;
        }
        else
        {
            /* Do Nothing */
        }

        i++;
        
        if( i >= sCompareInfo->mKeyColCount )
        {
            sCompareInfo->mUniqueViolated += 1;
        
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrA, &sPageIdA );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrA, &sOffsetA );
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrB, &sPageIdB );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrB, &sOffsetB );
                
            if( sPageIdA > sPageIdB )
            {
                sResult = DTL_COMPARISON_GREATER;
            }
            else if( sPageIdA < sPageIdB )
            {
                sResult = DTL_COMPARISON_LESS;
            }
            else
            {
                if( sOffsetA > sOffsetB )
                {
                    sResult = DTL_COMPARISON_GREATER;
                }
                else if( sOffsetA < sOffsetB )
                {
                    sResult = DTL_COMPARISON_LESS;
                }
                else
                {
                    sResult = DTL_COMPARISON_EQUAL;
                    sCompareInfo->mUniqueViolated -= 1;
                }
            }
            
            break;
        }

        sKeyA += (sColLenSizeA + sColLenA);
        sKeyB += (sColLenSizeB + sColLenB);

    } while( 1 );

    return sResult;
}

stlInt32 smnmpbCompare1MergeNonPrimary( void * aCompareInfo,
                                        void * aValueA,
                                        void * aValueB )
{
    smnmpbCompareInfo * sCompareInfo = (smnmpbCompareInfo*)aCompareInfo;
    stlChar           * sKeyA;
    stlChar           * sKeyB;
    stlChar           * sKeyHdrA;
    stlChar           * sKeyHdrB;
    stlUInt8            sColLenSizeA = 1;
    stlInt64            sColLenA;
    stlUInt8            sColLenSizeB = 1;
    stlInt64            sColLenB;
    dtlCompareResult    sResult = DTL_COMPARISON_EQUAL;
    stlInt8           * sKeyOrder;
    stlUInt8          * sKeyFlags;
    smlPid              sPageIdA;
    smlPid              sPageIdB;
    smpOffsetSlot       sOffsetA;
    smpOffsetSlot       sOffsetB;
    stlInt32            i = 0;
    stlInt32            sNullColCnt = 0;
    dtlCompArg          sCompArg;

    sKeyOrder = sCompareInfo->mKeyColOrder;
    sKeyFlags = sCompareInfo->mKeyColFlags;

    sKeyHdrA = (stlChar*)aValueA;
    sKeyHdrB = (stlChar*)aValueB;

    sKeyA = sKeyHdrA + SMNMPB_RUN_KEY_HDR_LEN;
    sKeyB = sKeyHdrB + SMNMPB_RUN_KEY_HDR_LEN;

    do
    {
        sColLenA = *(stlUInt8*)sKeyA;
        sColLenB = *(stlUInt8*)sKeyB;
        
        if( sColLenA == 0 )
        {
            sNullColCnt++;
            
            /**
             * value1이 NULL
             */
            if( sColLenB == 0 )
            {
                /* value1(NULL) = value2(NULL) */
                sResult = DTL_COMPARISON_EQUAL;
            }
            else if( (sKeyFlags[i] & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                     == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
            {
                /* value1(NULL) < value2(NOTNULL) */
                sResult = DTL_COMPARISON_LESS;
                break;
            }
            else 
            {
                /* value1(NULL) > value2(NOTNULL) */
                sResult = DTL_COMPARISON_GREATER;
                break;
            }
        }
        else
        {
            /**
             * value1이 NOT NULL
             */
            if( sColLenB == 0 )
            {
                if( (sKeyFlags[i] & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                    == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
                {                    
                    /* value1(NOTNULL) > value2(NULL) */
                    sResult = DTL_COMPARISON_GREATER;
                    break;
                }
                else 
                {
                    /* value1(NOTNULL) < value2(NULL) */
                    sResult = DTL_COMPARISON_LESS;
                    break;
                }
            }
            else 
            {
                /**
                 * value1(NOTNULL) & value2(NOTNULL)
                 */

                DTL_TYPED_COMPARE( sCompareInfo->mPhysicalCompare[i],
                                   sResult,
                                   sCompArg,
                                   sKeyA + sColLenSizeA,
                                   sColLenA,
                                   sKeyB + sColLenSizeB,
                                   sColLenB );

                if( sResult != DTL_COMPARISON_EQUAL )
                {
                    /**
                     * Key Ordering 보정
                     */
                    sResult *= sKeyOrder[i];
                    break;
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }

        i++;
        
        if( i >= sCompareInfo->mKeyColCount )
        {
            sCompareInfo->mUniqueViolated += ( sCompareInfo->mKeyColCount != sNullColCnt );
        
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrA, &sPageIdA );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrA, &sOffsetA );
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrB, &sPageIdB );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrB, &sOffsetB );
                
            if( sPageIdA > sPageIdB )
            {
                sResult = DTL_COMPARISON_GREATER;
            }
            else if( sPageIdA < sPageIdB )
            {
                sResult = DTL_COMPARISON_LESS;
            }
            else
            {
                if( sOffsetA > sOffsetB )
                {
                    sResult = DTL_COMPARISON_GREATER;
                }
                else if( sOffsetA < sOffsetB )
                {
                    sResult = DTL_COMPARISON_LESS;
                }
                else
                {
                    sResult = DTL_COMPARISON_EQUAL;
                    sCompareInfo->mUniqueViolated -= ( sCompareInfo->mKeyColCount != sNullColCnt );
                }
            }
            
            break;
        }

        sKeyA += (sColLenSizeA + sColLenA);
        sKeyB += (sColLenSizeB + sColLenB);

    } while( 1 );

    return sResult;
}

stlInt32 smnmpbCompare1MergePrimary( void * aCompareInfo,
                                     void * aValueA,
                                     void * aValueB )
{
    smnmpbCompareInfo * sCompareInfo = (smnmpbCompareInfo*)aCompareInfo;
    stlChar           * sKeyA;
    stlChar           * sKeyB;
    stlChar           * sKeyHdrA;
    stlChar           * sKeyHdrB;
    stlUInt8            sColLenSizeA = 1;
    stlInt64            sColLenA;
    stlUInt8            sColLenSizeB = 1;
    stlInt64            sColLenB;
    dtlCompareResult    sResult = DTL_COMPARISON_EQUAL;
    stlInt8           * sKeyOrder;
    smlPid              sPageIdA;
    smlPid              sPageIdB;
    smpOffsetSlot       sOffsetA;
    smpOffsetSlot       sOffsetB;
    stlInt32            i = 0;
    dtlCompArg          sCompArg;

    sKeyOrder = sCompareInfo->mKeyColOrder;

    sKeyHdrA = (stlChar*)aValueA;
    sKeyHdrB = (stlChar*)aValueB;

    sKeyA = sKeyHdrA + SMNMPB_RUN_KEY_HDR_LEN;
    sKeyB = sKeyHdrB + SMNMPB_RUN_KEY_HDR_LEN;

    do
    {
        sColLenA = *(stlUInt8*)sKeyA;
        sColLenB = *(stlUInt8*)sKeyB;
        
        DTL_TYPED_COMPARE( sCompareInfo->mPhysicalCompare[i],
                           sResult,
                           sCompArg,
                           sKeyA + sColLenSizeA,
                           sColLenA,
                           sKeyB + sColLenSizeB,
                           sColLenB );

        if( sResult != DTL_COMPARISON_EQUAL )
        {
            /**
             * Key Ordering 보정
             */
            sResult *= sKeyOrder[i];
            break;
        }
        else
        {
            /* Do Nothing */
        }

        i++;
        
        if( i >= sCompareInfo->mKeyColCount )
        {
            sCompareInfo->mUniqueViolated += 1;
        
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrA, &sPageIdA );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrA, &sOffsetA );
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrB, &sPageIdB );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrB, &sOffsetB );
                
            if( sPageIdA > sPageIdB )
            {
                sResult = DTL_COMPARISON_GREATER;
            }
            else if( sPageIdA < sPageIdB )
            {
                sResult = DTL_COMPARISON_LESS;
            }
            else
            {
                if( sOffsetA > sOffsetB )
                {
                    sResult = DTL_COMPARISON_GREATER;
                }
                else if( sOffsetA < sOffsetB )
                {
                    sResult = DTL_COMPARISON_LESS;
                }
                else
                {
                    sResult = DTL_COMPARISON_EQUAL;
                    sCompareInfo->mUniqueViolated -= 1;
                }
            }
            
            break;
        }

        sKeyA += (sColLenSizeA + sColLenA);
        sKeyB += (sColLenSizeB + sColLenB);

    } while( 1 );

    return sResult;
}

stlInt32 smnmpbCompareNMergeNonPrimary( void * aCompareInfo,
                                        void * aValueA,
                                        void * aValueB )
{
    smnmpbCompareInfo * sCompareInfo = (smnmpbCompareInfo*)aCompareInfo;
    stlChar           * sKeyA;
    stlChar           * sKeyB;
    stlChar           * sKeyHdrA;
    stlChar           * sKeyHdrB;
    stlUInt8            sColLenSizeA;
    stlInt64            sColLenA;
    stlUInt8            sColLenSizeB;
    stlInt64            sColLenB;
    dtlCompareResult    sResult = DTL_COMPARISON_EQUAL;
    stlInt8           * sKeyOrder;
    stlUInt8          * sKeyFlags;
    smlPid              sPageIdA;
    smlPid              sPageIdB;
    smpOffsetSlot       sOffsetA;
    smpOffsetSlot       sOffsetB;
    stlInt32            i = 0;
    stlInt32            sNullColCnt = 0;
    dtlCompArg          sCompArg;

    sKeyOrder = sCompareInfo->mKeyColOrder;
    sKeyFlags = sCompareInfo->mKeyColFlags;

    sKeyHdrA = (stlChar*)aValueA;
    sKeyHdrB = (stlChar*)aValueB;

    sKeyA = sKeyHdrA + SMNMPB_RUN_KEY_HDR_LEN;
    sKeyB = sKeyHdrB + SMNMPB_RUN_KEY_HDR_LEN;

    do
    {
        SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyA, &sColLenSizeA, &sColLenA );
        SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyB, &sColLenSizeB, &sColLenB );
        
        if( sColLenA == 0 )
        {
            sNullColCnt++;
            
            /**
             * value1이 NULL
             */
            if( sColLenB == 0 )
            {
                /* value1(NULL) = value2(NULL) */
                sResult = DTL_COMPARISON_EQUAL;
            }
            else if( (sKeyFlags[i] & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                     == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
            {
                /* value1(NULL) < value2(NOTNULL) */
                sResult = DTL_COMPARISON_LESS;
                break;
            }
            else 
            {
                /* value1(NULL) > value2(NOTNULL) */
                sResult = DTL_COMPARISON_GREATER;
                break;
            }
        }
        else
        {
            /**
             * value1이 NOT NULL
             */
            if( sColLenB == 0 )
            {
                if( (sKeyFlags[i] & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                    == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
                {                    
                    /* value1(NOTNULL) > value2(NULL) */
                    sResult = DTL_COMPARISON_GREATER;
                    break;
                }
                else 
                {
                    /* value1(NOTNULL) < value2(NULL) */
                    sResult = DTL_COMPARISON_LESS;
                    break;
                }
            }
            else 
            {
                /**
                 * value1(NOTNULL) & value2(NOTNULL)
                 */

                DTL_TYPED_COMPARE( sCompareInfo->mPhysicalCompare[i],
                                   sResult,
                                   sCompArg,
                                   sKeyA + sColLenSizeA,
                                   sColLenA,
                                   sKeyB + sColLenSizeB,
                                   sColLenB );

                if( sResult != DTL_COMPARISON_EQUAL )
                {
                    /**
                     * Key Ordering 보정
                     */
                    sResult *= sKeyOrder[i];
                    break;
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }

        i++;
        
        if( i >= sCompareInfo->mKeyColCount )
        {
            sCompareInfo->mUniqueViolated += ( sCompareInfo->mKeyColCount != sNullColCnt );
        
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrA, &sPageIdA );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrA, &sOffsetA );
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrB, &sPageIdB );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrB, &sOffsetB );
                
            if( sPageIdA > sPageIdB )
            {
                sResult = DTL_COMPARISON_GREATER;
            }
            else if( sPageIdA < sPageIdB )
            {
                sResult = DTL_COMPARISON_LESS;
            }
            else
            {
                if( sOffsetA > sOffsetB )
                {
                    sResult = DTL_COMPARISON_GREATER;
                }
                else if( sOffsetA < sOffsetB )
                {
                    sResult = DTL_COMPARISON_LESS;
                }
                else
                {
                    sResult = DTL_COMPARISON_EQUAL;
                    sCompareInfo->mUniqueViolated -= ( sCompareInfo->mKeyColCount != sNullColCnt );
                }
            }
            
            break;
        }

        sKeyA += (sColLenSizeA + sColLenA);
        sKeyB += (sColLenSizeB + sColLenB);

    } while( 1 );

    return sResult;
}

stlInt32 smnmpbCompareNMergePrimary( void * aCompareInfo,
                                     void * aValueA,
                                     void * aValueB )
{
    smnmpbCompareInfo * sCompareInfo = (smnmpbCompareInfo*)aCompareInfo;
    stlChar           * sKeyA;
    stlChar           * sKeyB;
    stlChar           * sKeyHdrA;
    stlChar           * sKeyHdrB;
    stlUInt8            sColLenSizeA;
    stlInt64            sColLenA;
    stlUInt8            sColLenSizeB;
    stlInt64            sColLenB;
    dtlCompareResult    sResult = DTL_COMPARISON_EQUAL;
    stlInt8           * sKeyOrder;
    smlPid              sPageIdA;
    smlPid              sPageIdB;
    smpOffsetSlot       sOffsetA;
    smpOffsetSlot       sOffsetB;
    stlInt32            i = 0;
    dtlCompArg          sCompArg;

    sKeyOrder = sCompareInfo->mKeyColOrder;

    sKeyHdrA = (stlChar*)aValueA;
    sKeyHdrB = (stlChar*)aValueB;

    sKeyA = sKeyHdrA + SMNMPB_RUN_KEY_HDR_LEN;
    sKeyB = sKeyHdrB + SMNMPB_RUN_KEY_HDR_LEN;

    do
    {
        SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyA, &sColLenSizeA, &sColLenA );
        SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyB, &sColLenSizeB, &sColLenB );
        
        DTL_TYPED_COMPARE( sCompareInfo->mPhysicalCompare[i],
                           sResult,
                           sCompArg,
                           sKeyA + sColLenSizeA,
                           sColLenA,
                           sKeyB + sColLenSizeB,
                           sColLenB );

        if( sResult != DTL_COMPARISON_EQUAL )
        {
            /**
             * Key Ordering 보정
             */
            sResult *= sKeyOrder[i];
            break;
        }
        else
        {
            /* Do Nothing */
        }

        i++;
        
        if( i >= sCompareInfo->mKeyColCount )
        {
            sCompareInfo->mUniqueViolated += 1;

            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrA, &sPageIdA );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrA, &sOffsetA );
            SMNMPB_RUN_KEY_GET_ROW_PID( sKeyHdrB, &sPageIdB );
            SMNMPB_RUN_KEY_GET_ROW_OFFSET( sKeyHdrB, &sOffsetB );
                
            if( sPageIdA > sPageIdB )
            {
                sResult = DTL_COMPARISON_GREATER;
            }
            else if( sPageIdA < sPageIdB )
            {
                sResult = DTL_COMPARISON_LESS;
            }
            else
            {
                if( sOffsetA > sOffsetB )
                {
                    sResult = DTL_COMPARISON_GREATER;
                }
                else if( sOffsetA < sOffsetB )
                {
                    sResult = DTL_COMPARISON_LESS;
                }
                else
                {
                    sResult = DTL_COMPARISON_EQUAL;
                    sCompareInfo->mUniqueViolated -= 1;
                }
            }
            
            break;
        }

        sKeyA += (sColLenSizeA + sColLenA);
        sKeyB += (sColLenSizeB + sColLenB);

    } while( 1 );

    return sResult;
}

knlCompareFunc gSmnmpbCompareFuncs[2][2][2] =
{
    /**< Quick Sort */
    {
        /**< 1 Byte Column Length */
        {
            smnmpbCompare1SortNonPrimary,   /**< Non-Primary */
            smnmpbCompare1SortPrimary       /**< Primary */
        },
        
        /**< N Byte Column Length */
        {
            smnmpbCompareNSortNonPrimary,   /**< Non-Primary */
            smnmpbCompareNSortPrimary       /**< Primary */
        }
    },
    
    /**< Merge Sort */
    {
        /**< 1 Byte Column Length */
        {
            smnmpbCompare1MergeNonPrimary,   /**< Non-Primary */
            smnmpbCompare1MergePrimary       /**< Primary */
        },
        
        /**< N Byte Column Length */
        {
            smnmpbCompareNMergeNonPrimary,   /**< Non-Primary */
            smnmpbCompareNMergePrimary       /**< Primary */
        }
    }
};

knlCompareFunc smnmpbGetCompareFunc( stlInt32 sCompareType,
                                     stlBool  s1ByteColLenSize,
                                     stlBool  sPrimaryIndex )
{
    knlCompareFunc sCompareFunc;
    
    if( s1ByteColLenSize == STL_TRUE )
    {
        if( sPrimaryIndex == STL_TRUE )
        {
            sCompareFunc = gSmnmpbCompareFuncs[sCompareType][0][1];
        }
        else
        {
            sCompareFunc = gSmnmpbCompareFuncs[sCompareType][0][0];
        }
    }
    else
    {
        if( sPrimaryIndex == STL_TRUE )
        {
            sCompareFunc = gSmnmpbCompareFuncs[sCompareType][1][1];
        }
        else
        {
            sCompareFunc = gSmnmpbCompareFuncs[sCompareType][1][0];
        }
    }

    return sCompareFunc;
}

stlInt32 smnmpbCompare4MergeTree( void * aCompareInfo,
                                  void * aValueA,
                                  void * aValueB )
{
    smnmpbCompareInfo * sCompareInfo = (smnmpbCompareInfo*)aCompareInfo;
    stlChar           * sKeyA;
    stlChar           * sKeyB;
    stlUInt8            sColLenSizeA;
    stlInt64            sColLenA;
    stlUInt8            sColLenSizeB;
    stlInt64            sColLenB;
    dtlCompareResult    sResult = DTL_COMPARISON_EQUAL;
    stlInt8           * sKeyOrder;
    stlUInt8          * sKeyFlags;
    stlInt32            i = 0;
    stlInt64            sNullColCnt = 0;
    dtlCompArg          sCompArg;

    sKeyOrder = sCompareInfo->mKeyColOrder;
    sKeyFlags = sCompareInfo->mKeyColFlags;

    sKeyA = aValueA;
    sKeyB = aValueB;

    do
    {
        SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyA, &sColLenSizeA, &sColLenA );
        SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyB, &sColLenSizeB, &sColLenB );
        
        if( sColLenA == 0 )
        {
            sNullColCnt++;

            /**
             * value1이 NULL
             */
            if( sColLenB == 0 )
            {
                /* value1(NULL) = value2(NULL) */
                sResult = DTL_COMPARISON_EQUAL;
            }
            else if( (sKeyFlags[i] & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                     == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
            {
                /* value1(NULL) < value2(NOTNULL) */
                sResult = DTL_COMPARISON_LESS;
                break;
            }
            else 
            {
                /* value1(NULL) > value2(NOTNULL) */
                sResult = DTL_COMPARISON_GREATER;
                break;
            }
        }
        else
        {
            /**
             * value1이 NOT NULL
             */
            if( sColLenB == 0 )
            {
                if( (sKeyFlags[i] & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                    == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
                {                    
                    /* value1(NOTNULL) > value2(NULL) */
                    sResult = DTL_COMPARISON_GREATER;
                    break;
                }
                else 
                {
                    /* value1(NOTNULL) < value2(NULL) */
                    sResult = DTL_COMPARISON_LESS;
                    break;
                }
            }
            else 
            {
                /**
                 * value1(NOTNULL) & value2(NOTNULL)
                 */

                DTL_TYPED_COMPARE( sCompareInfo->mPhysicalCompare[i],
                                   sResult,
                                   sCompArg,
                                   sKeyA + sColLenSizeA,
                                   sColLenA,
                                   sKeyB + sColLenSizeB,
                                   sColLenB );

                if( sResult != DTL_COMPARISON_EQUAL )
                {
                    /**
                     * Key Ordering 보정
                     */
                    sResult *= sKeyOrder[i];
                    break;
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }

        i++;
        
        if( i >= sCompareInfo->mKeyColCount )
        {
            sCompareInfo->mUniqueViolated += ( sCompareInfo->mKeyColCount != sNullColCnt );
            break;
        }
        
        sKeyA += (sColLenSizeA + sColLenA);
        sKeyB += (sColLenSizeB + sColLenB);

    } while( 1 );

    return sResult;
}

/** @} */
