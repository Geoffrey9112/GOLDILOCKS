/*******************************************************************************
 * ellHashValue.c
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
 * @file ellHashValue.c
 * @brief Hash Value 계산 함수 
 */

#include <ell.h>
#include <ellHashValue.h>


/**
 * @addtogroup ellHashValue
 * @{
 */

/**
 * Hash Value 계산 from postgres
 */
stlUInt32  ellHashInt64( stlInt64 aID )
{
    stlUInt32  sVal = 0;
    
    sVal = 0x9e3779b9 + (stlUInt32) sizeof(stlUInt32) + 3923095;
    sVal = sVal + (stlUInt32)aID;
    
    return sVal;
}


/**
 * Hash Value 계산 from postgres
 */
stlUInt32  ellHashString( stlChar * aName )
{
    stlSize   sLen = 0;
    stlChar * sStr = NULL;
    stlUInt32  i;
    stlUInt32  j;
    stlUInt32  k;

    sLen = stlStrlen( aName );
    k = 0x9e3779b9 + sLen + 3923095;
    
    i = j = k;
    sStr = aName;
    
    while ( sLen >= 12 )
    {
		i += ( sStr[3] +
               ((stlUInt32) sStr[2]  << 8) +
               ((stlUInt32) sStr[1]  << 16) +
               ((stlUInt32) sStr[0]  << 24) );
		j += ( sStr[7] +
               ((stlUInt32) sStr[6]  << 8) +
               ((stlUInt32) sStr[5]  << 16) +
               ((stlUInt32) sStr[4]  << 24) );
		k += ( sStr[11] +
               ((stlUInt32) sStr[10] << 8) +
               ((stlUInt32) sStr[9]  << 16) +
               ((stlUInt32) sStr[8]  << 24) );
        sStr += 12;
        sLen -= 12;
    }

    switch ( sLen )
    {
        case 11:
            k += ((stlUInt32) sStr[10] << 8);
        case 10:
            k += ((stlUInt32) sStr[9] << 16);
        case 9:
            k += ((stlUInt32) sStr[8] << 24);
        case 8:
            j += sStr[7];
        case 7:
            j += ((stlUInt32) sStr[6] << 8);
        case 6:
            j += ((stlUInt32) sStr[5] << 16);
        case 5:
            i += ((stlUInt32) sStr[4] << 24);
        case 4:
            i += sStr[3];
        case 3:
            i += ((stlUInt32) sStr[2] << 8);
        case 2:
            i += ((stlUInt32) sStr[1] << 16);
        case 1:
            i += ((stlUInt32) sStr[0] << 24);
        case 0:
            /* nothing left */
        default:
            break;
    }
    
    k ^= j;
    k -= ellROT(j,14);
    
    i ^= k;
    i -= ellROT(k,11);
    
    j ^= i;
    j -= ellROT(i,25);
    
    k ^= j;
    k -= ellROT(j,16);
    
    i ^= k;
    i -= ellROT(k, 4);
    
    j ^= i;
    j -= ellROT(i,14);
    
    k ^= j;
    k -= ellROT(j,24);

    return k;
}

/**
 * @brief 하나의 ID 로부터 Hash Value 를 얻는다.
 * @param[in]  aObjID   Object ID
 * @remarks
 * Object ID는 객체 유형중에서 유일한 값이다.
 * 별도의 hash 계산을 할 필요가 없다.
 */
stlUInt32  ellGetHashValueOneID( stlInt64 aObjID )
{
    return (stlUInt32) aObjID;
}

/**
 * @brief 두개의 ID 로부터 Hash Value 를 얻는다.
 * @param[in]  aObjID1   Object ID
 * @param[in]  aObjID2   Object ID
 * @remarks
 * from postgres
 */
stlUInt32  ellGetHashValueTwoID( stlInt64 aObjID1, stlInt64 aObjID2 )
{
    stlUInt32   sHashValue = 0;
    stlUInt32   sOneValue;

    sOneValue = ellHashInt64( aObjID1 );
    
    sHashValue ^= sOneValue << 8;
    sHashValue ^= sOneValue >> 24;

    sOneValue = ellHashInt64( aObjID2 );

    sHashValue ^= sOneValue;
    
    return sHashValue;
}

/**
 * @brief Object Name 으로부터 Hash Value 를 얻는다.
 * @param[in]  aObjName   Object Name
 * @remarks
 * from postgres
 */
stlUInt32  ellGetHashValueOneName( stlChar * aObjName )
{
    stlUInt32  sHashValue = 0;

    sHashValue = ellHashString( aObjName );

    return sHashValue;
}

/**
 * @brief Object ID 와 Object Name 으로부터 Hash Value 를 얻는다.
 * @param[in]  aObjID     Object ID
 * @param[in]  aObjName   Object Name
 * @remarks
 * from postgres
 */
stlUInt32  ellGetHashValueNameAndID( stlInt64 aObjID, stlChar * aObjName )
{
    stlUInt32   sHashValue = 0;
    stlUInt32   sOneValue;

    sOneValue = ellHashInt64( aObjID );
    
    sHashValue ^= sOneValue << 8;
    sHashValue ^= sOneValue >> 24;

    sOneValue = ellHashString( aObjName );

    sHashValue ^= sOneValue;

    return sHashValue;
}

/**
 * @brief Record Count 로부터 솟수 Bucket Count 를 계산
 * @param[in] aRecordCnt Record Count
 * @remarks
 */
stlUInt32  ellGetPrimeBucketCnt( stlInt64 aRecordCnt )
{
    stlInt32  i = 0;
    
    stlUInt32 sBucketCnt = 0;

    /**
     * Gutenberg EBook 프로젝트의 솟수 찾기 프로젝트에서 임의의 솟수를 발췌함
     * 참조 ) http://www.gutenberg.org/cache/epub/65/pg65.html.utf8
     */
    stlUInt32 sPrimeNO[] =
        {
            127,   /* 최소값 */
            337,
            563,
            727,
            953,
            1031,
            3181,
            5077,
            7673,
            9749,
            10979,
            20789,
            33287,
            43591,
            54059,
            65537, /* 최대값 */
            0,     /* 종료값 */
        };

    /**
     * Record 개수보다 큰 소수값을 선택함.
     * 최대값보다 Record 개수가 크다면 최대값을 선택함.
     */
    for ( i = 0; sPrimeNO[i] != 0; i++ )
    {
        sBucketCnt = sPrimeNO[i];

        if ( sBucketCnt > aRecordCnt )
        {
            break;
        }
        else
        {
            continue;
        }
    }
    
    return sBucketCnt;
}

/** @} ellHashValue */
