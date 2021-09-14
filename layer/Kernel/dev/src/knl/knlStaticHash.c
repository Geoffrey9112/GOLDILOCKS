/*******************************************************************************
 * knlStaticHash.c
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
 * @file knlStaticHash.c
 * @brief Kernel Layer Static Hash wrapper routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knaLatch.h>
#include <knhStaticHash.h>

/**
 * Gutenberg EBook 프로젝트의 솟수 찾기 프로젝트에서 임의의 솟수를 발췌함
 * 참조 ) http://www.gutenberg.org/cache/epub/65/pg65.html.utf8
 *
 * 16 bit Modular Constant 구하는 방법
 *   1.  Convert 1 / K into binary.
 *   2.  Take all the bits to the right of the binary point, and left shift them until
 *       the bit to the right of the binary point is 1. Record the required number of shifts S.
 *   3.  Take the most significant 18 bits and add 1 and then truncate to 17 bits.
 *       This effectively rounds the result.
 *   4.  Express the 17 bit result as 1hhhh. Denote the hhhh portion as M
 *   5.  Q = ((((uint32_t)A * (uint32_t)M) >> 16) + A) >> 1) >> S
 *   6.  Perform an exhaustive check for all A & Q. If necessary adjust M.
 *
 *  Quotent = ( Prime Number * constant ) >> shift bit
 *  Remain  = HashKey - ( Quotent * Prime Number )
 *  
 *  참조 ) http://embeddedgurus.com/stack-overflow/tag/division/
 */
    
knlHashModulus gKnlHashModulus[] =
{
    { 127,     0x81020408,   6, 1 },  /* 최소값 */
    { 337,     0xC2780614,   8, 1 },
    { 563,     0xE8CF58AB,   9, 1 },
    { 727,     0xB44AA6E9,   9, 1 },
    { 953,     0x89894480,   9, 0 },
    { 1031,    0xFE430AAD,  10, 1 },
    { 3181,    0xA4D19075,  11, 1 },
    { 5077,    0xCE88D96D,  12, 1 },
    { 7673,    0x88A86B91,  12, 0 },
    { 9749,    0xD71D54D8,  13, 1 },
    { 10979,   0xBF03CCFB,  13, 1 },
    { 20789,   0xC9C1848A,  14, 0 },
    { 33287,   0xFC022F25,  15, 1 },
    { 43591,   0xC07061A1,  15, 1 },
    { 54059,   0x9B2CD00B,  15, 1 },
    { 65537,   0xFFFF0001,  16, 1 },
    { 77977,   0xD727ECC0,  16, 0 },
    { 87119,   0xC09401C0,  16, 1 },
    { 98429,   0xAA732E82,  16, 0 },
    { 199999,  0xA7C5E341,  17, 1 },
    { 299749,  0xDFE22EF9,  18, 0 },
    { 399989,  0xA7C6DAA7,  18, 1 },
    { 499979,  0x86392E79,  18, 1 },
    { 599993,  0xDFB2E613,  19, 1 },
    { 699967,  0xBFBFA798,  19, 0 },
    { 799999,  0xA7C5BA06,  19, 0 },
    { 899981,  0x95224B06,  19, 1 },
    { 999983,  0x8638528F,  19, 0 },
    { 1099997, 0xF4086C05,  20, 1 },
    { 0, 0, 0, 0 }   /* 종료조건 */
};
    
/**
 * @brief Static, Fixed 메모리 영역에 knlStaticHash 타입의 해쉬를 생성한다
 * @param[out] aHash Hash가 저장된 공간의 시작 주소
 * @param[in] aBucketCount hash가 가질 bucket의 개수
 * @param[in] aLinkOffset hash에 달릴 데이터 구조체에서 stlRingEntry 타입의 멤버의 Offset
 * @param[in] aKeyOffset hash에 달릴 데이터 구조체에서 Key로 사용되는 멤버의 Offset
 * @param[in] aIsOnShm hash가 shared memory 영역에 생성되어야 하는지 여부
 * @param[in] aMemMgr hash가 heap 영역에 생성될 경우(aIsOnShm == STL_FALSE)에 사용되는 메모리 관리자
 * @param[in] aLatchMode hash에서 사용할 동시성 정도
 * <PRE>
 * KNL_STATICHASH_MODE_NOLATCH
 * KNL_STATICHASH_MODE_HASHLATCH
 * KNL_STATICHASH_MODE_BUCKETLATCH
 * </PRE>
 * @param[out] aEnv environment 정보
 */
stlStatus knlCreateStaticHash( knlStaticHash ** aHash, 
                               stlUInt16        aBucketCount,
                               stlUInt16        aLinkOffset,
                               stlUInt16        aKeyOffset,
                               stlUInt32        aLatchMode,
                               stlBool          aIsOnShm,
                               knlRegionMem   * aMemMgr,
                               knlEnv         * aEnv )
{
    return knhCreateStaticHash( aHash, 
                                aBucketCount,
                                aLinkOffset,
                                aKeyOffset,
                                aLatchMode,
                                aIsOnShm,
                                aMemMgr,
                                aEnv );
}

/**
 * @brief 주어진 Hash에 데이터를 삽입한다
 * @param[in,out] aHash Hash 정보를 저장한 공간의 시작 주소
 * @param[in] aHashFunc key를 이용하여 Bucket 번호를 알아내기위한 Hashing 함수
 * @param[in] aData hash에 저장할 Key를 가진 데이터 구조체 포인터
 * @param[out] aEnv environment 정보
 */
stlStatus knlInsertStaticHash( knlStaticHash            * aHash, 
                               knlStaticHashHashingFunc   aHashFunc,
                               void                     * aData,
                               knlEnv                   * aEnv )
{
    return knhInsertStaticHash( aHash, 
                                aHashFunc,
                                aData,
                                aEnv );
}

/**
 * @brief 주어진 Hash에서 데이터를 삭제한다
 * @param[in,out] aHash Hash 정보를 저장한 공간의 시작 주소
 * @param[in] aHashFunc key를 이용하여 Bucket 번호를 알아내기위한 Hashing 함수
 * @param[in] aData hash에서 삭제할 Key를 가진 데이터 구조체 포인터
 * @param[out] aEnv environment 정보
 */
stlStatus knlRemoveStaticHash( knlStaticHash            * aHash, 
                               knlStaticHashHashingFunc   aHashFunc,
                               void                     * aData,
                               knlEnv                   * aEnv )
{
    return knhRemoveStaticHash( aHash, 
                                aHashFunc,
                                aData,
                                aEnv );
}

/**
 * @brief 주어진 Hash에서 key에 해당하는 데이터를 찾아낸다
 * @param[in] aHash Hash 정보를 저장한 공간의 시작 주소
 * @param[in] aHashFunc key를 이용하여 Bucket 번호를 알아내기위한 Hashing 함수
 * @param[in] aCompareFunc key값끼리 비교하기 위한 함수
 * @param[in] aKey 찾을 key값
 * @param[out] aData 주어진 Key를 가진 데이터 구조체 포인터
 * @param[out] aEnv environment 정보
 */
stlStatus knlFindStaticHash( knlStaticHash             * aHash, 
                             knlStaticHashHashingFunc    aHashFunc,
                             knlStaticHashCompareFunc    aCompareFunc,
                             void                      * aKey,
                             void                     ** aData,
                             knlEnv                    * aEnv )
{
    return knhFindStaticHash( aHash, 
                              aHashFunc,
                              aCompareFunc,
                              aKey,
                              aData,
                              aEnv );
}

/**
 * @brief 주어진 Hash에 입력된 item들을 scan하기 위해 첫번째 아이템을 가져온다
 * @param[in] aHash Hash 정보를 저장한 공간의 시작 주소
 * @param[out] aFirstBucketSeq 첫번째 item을 가진 bucket의 번호
 * @param[out] aData 첫번째 item 데이터 구조체 포인터
 * @param[out] aEnv environment 정보
 */
stlStatus knlGetFirstStaticHashItem( knlStaticHash             * aHash, 
                                     stlUInt32                 * aFirstBucketSeq,
                                     void                     ** aData,
                                     knlEnv                    * aEnv )
{
    return knhGetFirstItem( aHash, 
                            aFirstBucketSeq,
                            aData,
                            aEnv );
}

/**
 * @brief 주어진 Hash에 입력된 item들중 주어진 item의 바로 다음 item을 가져온다
 * 없을 경우 NULL을 리턴받는다
 * @param[in] aHash Hash 정보를 저장한 공간의 시작 주소
 * @param[in,out] aBucketSeq 이전 item을 가진 bucket의 번호이며, 다음 item을 가진 bucket의 번호를 반환받는다
 * @param[in,out] aData 이전 item 데이터 구조체 포인터이며, 다음 item 데이터 구조체 포인터를 반환받는다
 * 다음 item이 없을 경우에는 NULL값을 가진채로 리턴된다
 * @param[out] aEnv environment 정보
 */
stlStatus knlGetNextStaticHashItem( knlStaticHash             * aHash, 
                                    stlUInt32                 * aBucketSeq,
                                    void                     ** aData,
                                    knlEnv                    * aEnv )
{
    return knhGetNextItem( aHash, 
                           aBucketSeq,
                           aData,
                           aEnv );
}

/** @} */
