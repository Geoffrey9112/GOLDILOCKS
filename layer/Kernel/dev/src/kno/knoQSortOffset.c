/*******************************************************************************
 * knoQSortOffset.c
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

/**
 * @file knoQSortOffset.c
 * @brief Kernel Layer Quick Sort for Offset List implementation routines
 */

#include <stlDef.h>
#include <dtl.h>
#include <knlDef.h>
#include <knoDef.h>


static void knoQSort2Way( stlUInt16       * aList,
                          stlInt32          aElemCount,
                          knlQSortContext * aContext )
{
    stlUInt16   * sStackList[KNO_QSORT_STACK_SIZE];
    stlInt32      sStackElemCount[KNO_QSORT_STACK_SIZE];
    stlInt32      sStackPos = -1;

    stlUInt16   * sList;
    stlInt32      sElemCount;

    stlInt32      i;
    stlInt32      j;
    stlUInt16     sSave;

    stlInt32      sMid;
    stlInt32      sCurLeft;
    stlInt32      sCurRight;
    stlUInt16     sPivotVal;

    /* Terminate on small subfiles */
    sList = aList;
    sElemCount = aElemCount;
    while( STL_TRUE )
    {
        if( sElemCount < KNO_QSORT_CUTOFF )
        {
            /**
             * element 개수가 KNO_QSORT_CUTOFF보다 작을 경우
             * quick sort보다 insertion sort가 더 효율적이므로
             * insertion sort로 처리하도록 한다.
             */
            for( i = 1; i < sElemCount; i++ )
            {
                if( aContext->mCompare(aContext->mCmpInfo, (void*)&sList[i - 1], (void*)&sList[i]) <= 0 )
                {
                    continue;
                }

                sSave = sList[i];
                sList[i] = sList[i - 1];

                for( j = i - 1; j > 0; j-- )
                {
                    if( aContext->mCompare(aContext->mCmpInfo, (void*)&sList[j - 1], (void*)&sSave) <= 0 )
                    {
                        break;
                    }
                    sList[j] = sList[j - 1];
                }

                sList[j] = sSave;
            }
        }
        else
        {
            /**
             * ascending으로 정렬된 데이터가 들어오는 경우
             * first value를 pivot value로 설정하게 되면
             * 데이터 개수만큼 loop를 돌아야 하는 경우가 발생한다.
             * 이는 최악의 성능을 가져오게 되는데, 이를 방지하기 위하여
             * first value, mid value, last value들에 대하여
             * 정렬상 중간값을 pivot value로 한다.
             */
            /* Find the median of three values in list, use it as the pivot */
            sMid = sElemCount / 2;

            if( aContext->mCompare(aContext->mCmpInfo, (void*)&sList[0], (void*)&sList[sMid]) > 0 )
            {
                KNO_QSORT_SWAP_OFFSET( sList[0], sList[sMid], sSave );
            }

            if( aContext->mCompare(aContext->mCmpInfo, (void*)&sList[0], (void*)&sList[sElemCount - 1]) > 0 )
            {
                KNO_QSORT_SWAP_OFFSET( sList[0], sList[sElemCount - 1], sSave );
            }

            if( aContext->mCompare(aContext->mCmpInfo, (void*)&sList[sMid], (void*)&sList[sElemCount - 1]) > 0 )
            {
                KNO_QSORT_SWAP_OFFSET( sList[sMid], sList[sElemCount - 1], sSave );
            }

            sPivotVal = sList[sMid];
            /**
             * 위에서 first, mid, last에 대한 정렬은 되었으므로
             * first와 last는 skip한다.
             */
            sCurLeft = 1;
            sCurRight = sElemCount - 2;

            /**
             * 2way Quick Sort를 수행한다.
             */
            /* Two way partition <=,,>= */
            while( sCurLeft <= sCurRight )
            {
                while( aContext->mCompare(aContext->mCmpInfo, (void*)&sList[sCurLeft], (void*)&sPivotVal) < 0 )
                {
                    sCurLeft++;
                };
                while( aContext->mCompare(aContext->mCmpInfo, (void*)&sList[sCurRight], (void*)&sPivotVal) > 0 )
                {
                    sCurRight--;
                }

                if( sCurLeft <= sCurRight )
                {
                    KNO_QSORT_SWAP_OFFSET( sList[sCurLeft], sList[sCurRight], sSave );
                    sCurLeft++;
                    sCurRight--;
                }
            }

            /**
             * pivot value를 기준으로
             * element count가 많은 쪽을 stack에 저장하고,
             * element count가 적은 쪽에 대해 quick sort를 계속 수행한다.
             * 반대로 할 경우 stack의 사용량이 더 많아지므로 주의한다.
             */
            STL_DASSERT( sStackPos + 1 < KNO_QSORT_STACK_SIZE );
            if( (sCurRight + 1) < (sElemCount - sCurLeft) )
            {
                /* pivot을 기준으로 left의 elem count가 적은 경우 */
                sStackPos++;
                sStackList[sStackPos]       = &sList[sCurLeft];
                sStackElemCount[sStackPos]  = sElemCount - sCurLeft;

                sElemCount = sCurRight + 1;
                continue;
            }
            else
            {
                /* pivot을 기준으로 right의 elem count가 적은 경우 */
                sStackPos++;
                sStackList[sStackPos]       = sList;
                sStackElemCount[sStackPos]  = sCurRight + 1;

                sList = &sList[sCurLeft];
                sElemCount = sElemCount - sCurLeft;
                continue;
            }
        }

        /**
         * stack에 저장된 list 및 element count를 가져온다.
         */
        if( sStackPos < 0 )
        {
            break;
        }
        else
        {
            sList       = sStackList[sStackPos];
            sElemCount  = sStackElemCount[sStackPos];
            sStackPos--;
        }
    }
}

static void knoQSort3Way( stlUInt16       * aList,
                          stlInt32          aElemCount,
                          knlQSortContext * aContext )
{
    stlUInt16   * sStackList[KNO_QSORT_STACK_SIZE];
    stlInt32      sStackElemCount[KNO_QSORT_STACK_SIZE];
    stlInt32      sStackPos = -1;

    stlUInt16   * sList;
    stlInt32      sElemCount;

    stlInt32      i;
    stlInt32      j;
    stlUInt16     sSave;

    stlInt32      sMid;
    stlUInt16     sPivotVal;
    stlInt32      sCurLeft;
    stlInt32      sRet;
    stlInt32      sRightEqual;
    stlInt32      sLeftSmall;
    stlUInt32     sBigLen;
    stlUInt32     sEqualLen;
    stlUInt32     sMinLen;  

    /* Terminate on small subfiles */
    sList = aList;
    sElemCount = aElemCount;
    while( STL_TRUE )
    {
        if( sElemCount < KNO_QSORT_CUTOFF )
        {
            /**
             * element 개수가 KNO_QSORT_CUTOFF보다 작을 경우
             * quick sort보다 insertion sort가 더 효율적이므로
             * insertion sort로 처리하도록 한다.
             */
            for( i = 1; i < sElemCount; i++ )
            {
                if( aContext->mCompare(aContext->mCmpInfo, (void*)&sList[i - 1], (void*)&sList[i]) <= 0 )
                {
                    continue;
                }

                sSave = sList[i];
                sList[i] = sList[i - 1];

                for( j = i - 1; j > 0; j-- )
                {
                    if( aContext->mCompare(aContext->mCmpInfo, (void*)&sList[j - 1], (void*)&sSave) <= 0 )
                    {
                        break;
                    }
                    sList[j] = sList[j - 1];
                }

                sList[j] = sSave;
            }
        }
        else
        {
            /**
             * ascending으로 정렬된 데이터가 들어오는 경우
             * first value를 pivot value로 설정하게 되면
             * 데이터 개수만큼 loop를 돌아야 하는 경우가 발생한다.
             * 이는 최악의 성능을 가져오게 되는데, 이를 방지하기 위하여
             * first value, mid value, last value들에 대하여
             * 정렬상 중간값을 pivot value로 한다.
             */
            /* Find the median of three values in list, use it as the pivot */
            sMid = sElemCount / 2;

            if( aContext->mCompare(aContext->mCmpInfo, (void*)&sList[0], (void*)&sList[sMid]) > 0 )
            {
                KNO_QSORT_SWAP_OFFSET( sList[0], sList[sMid], sSave );
            }

            if( aContext->mCompare(aContext->mCmpInfo, (void*)&sList[0], (void*)&sList[sElemCount - 1]) > 0 )
            {
                KNO_QSORT_SWAP_OFFSET( sList[0], sList[sElemCount - 1], sSave );
            }

            if( aContext->mCompare(aContext->mCmpInfo, (void*)&sList[sElemCount - 1], (void*)&sList[sMid]) > 0 )
            {
                KNO_QSORT_SWAP_OFFSET( sList[sMid], sList[sElemCount - 1], sSave );
            }

            sPivotVal   = sList[sElemCount - 1];
            sCurLeft    = 0;
            sLeftSmall  = 0;
            sRightEqual = sElemCount - 1;

            /**
             * 3way Quick Sort를 수행한다.
             */
            /* Three way partition <,==,> */
            while( (sCurLeft < sElemCount - 1) && (sCurLeft < sRightEqual) )
            {
                sRet = aContext->mCompare(aContext->mCmpInfo, (void*)&sList[sCurLeft], (void*)&sPivotVal);
                if( sRet < 0 )
                {
                    if( sCurLeft != sLeftSmall )
                    {
                        KNO_QSORT_SWAP_OFFSET( sList[sCurLeft], sList[sLeftSmall], sSave );
                    }
                    sCurLeft++;
                    sLeftSmall++;
                }
                else if( sRet == 0 )
                {
                    sRightEqual--;
                    KNO_QSORT_SWAP_OFFSET( sList[sCurLeft], sList[sRightEqual], sSave );
                }
                else
                {
                    sCurLeft++;
                }
            }

            sEqualLen = sElemCount - sRightEqual;
            if( sEqualLen == sElemCount )
            {
                /* 모든 element가 같은 값임 */
                /* Do Nothing */
            }
            else
            {
                /**
                 * pivot value와 동일한 값을 가장 오른쪽에 채워놓았다.
                 * 여기서는 이를 pivot value보다 작은값과 큰값 사이로
                 * 옮기는 작업을 수행한다.
                 */
                sBigLen = sRightEqual - sLeftSmall;
                sMinLen = sBigLen > sEqualLen ? sEqualLen : sBigLen;

                for( i = 0; i < sMinLen; i++ )
                {
                    KNO_QSORT_SWAP_OFFSET( sList[sLeftSmall + i], sList[sElemCount  - 1 - i], sSave );
                }

                sCurLeft = sLeftSmall + sEqualLen;

                /**
                 * pivot value를 기준으로
                 * element count가 많은 쪽을 stack에 저장하고,
                 * element count가 적은 쪽에 대해 quick sort를 계속 수행한다.
                 * 반대로 할 경우 stack의 사용량이 더 많아지므로 주의한다.
                 */
                STL_DASSERT( sStackPos + 1 < KNO_QSORT_STACK_SIZE );
                if( sLeftSmall < (sElemCount - sCurLeft) )
                {
                    /* pivot을 기준으로 left의 elem count가 적은 경우 */
                    sStackPos++;
                    sStackList[sStackPos]       = &sList[sCurLeft];
                    sStackElemCount[sStackPos]  = sElemCount - sCurLeft;

                    sElemCount = sLeftSmall;
                    continue;
                }
                else
                {
                    /* pivot을 기준으로 right의 elem count가 적은 경우 */
                    sStackPos++;
                    sStackList[sStackPos]       = sList;
                    sStackElemCount[sStackPos]  = sLeftSmall;

                    sList = &sList[sCurLeft];
                    sElemCount = sElemCount - sCurLeft;
                    continue;
                }
            }
        }

        /**
         * stack에 저장된 list 및 element count를 가져온다.
         */
        if( sStackPos < 0 )
        {
            break;
        }
        else
        {
            sList       = sStackList[sStackPos];
            sElemCount  = sStackElemCount[sStackPos];
            sStackPos--;
        }
    }
}

void knoQSortOffset( stlUInt16       * aList,
                     stlInt32          aElemCount,
                     knlQSortContext * aContext )
{
    if( aContext->mIsUnique == STL_TRUE )
    {
        knoQSort2Way( aList, aElemCount, aContext );
    }
    else
    {
        knoQSort3Way( aList, aElemCount, aContext );
    }
}


/** @} */
