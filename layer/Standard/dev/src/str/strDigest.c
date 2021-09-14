/*******************************************************************************
 * strDigest.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: strDigest.c 368 2011-03-22 04:56:48Z lym999 $
 *
 * NOTES
 *    SHA1 알고리즘을 이용하여 string을 160bit(20byte)로 변환한다.
 *    특징
 *    - Digest함수이다. (입력 길이에 상관없이 output은 160bit(20byte) 이다.)
 *    - 단방향 함수이다. (원래 값으로 복원 불가.)
 *    - 향후 openssl을 사용하게 되면 openssl내부의 sha함수를 이용하도록
 *      변경하는것이 좋을것 같음.  (소스가 중복됨으로..)
 *
 *    NO COPYRIGHT - THIS IS 100% IN THE PUBLIC DOMAIN
 *    http://www.aarongifford.com/computers/sha.html
 *    ftp://ftp.funet.fi/pub/crypt/hash/sha/sha1.c
 *
 *    아래 값으로 검증함.
 *    input : "abc"
 *    sha1  : "a9993e364706816aba3e25717850c26c9cd0d89d"
 *    input : "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
 *    sha1  : "84983e441c3bd26ebaae4aa1f95129e5e54670f1"
 *
 ******************************************************************************/


/**
 * @file strDigest.c
 * @brief Standard Layer Digest(Sha1) Routines
 */

#include <str.h>
#include <strDigest.h>

/**
 * @addtogroup strDigest Digest
 * @{
 */

/*
 * 아래 define은 다른데서는 전혀 사용되지 않을것 같아서 source에 두었음.
 * 여기의 DEFINE 여기서만 사용됨으로 prefix붙이지 않았음.
 */

#define ROL(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */

#ifndef STL_IS_BIGENDIAN
#define BLK0(i) (sBlock->mL[i] = (ROL(sBlock->mL[i],24)&(stlUInt32)0xFF00FF00) \
    |(ROL(sBlock->mL[i],8)&(stlUInt32)0x00FF00FF))
#else
#define BLK0(i) sBlock->mL[i]
#endif

#define BLK(i) (sBlock->mL[i&15] = ROL(sBlock->mL[(i+13)&15]^sBlock->mL[(i+8)&15] \
    ^sBlock->mL[(i+2)&15]^sBlock->mL[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+BLK0(i)+0x5A827999+ROL(v,5);w=ROL(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+BLK(i)+0x5A827999+ROL(v,5);w=ROL(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+BLK(i)+0x6ED9EBA1+ROL(v,5);w=ROL(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+BLK(i)+0x8F1BBCDC+ROL(v,5);w=ROL(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+BLK(i)+0xCA62C1D6+ROL(v,5);w=ROL(w,30);

typedef union strByte64Quad16 {
    stlUInt8    mC[64];
    stlUInt32   mL[16];
} strByte64Quad16;

/**
 * CRC-32 Code Table
 * 참조
 * 1) http://en.wikipedia.org/wiki/Cyclic_redundancy_check
 * 2) http://www.csbruce.com/software/crc32.c 
 * POLYNOMIAL 0xEDB88320으로 미리 구한 CRC32 Table.
 * for( i = 0; i < 256; i++ )
 * {
 *     (unsigned long) CRC = (unsigned long) i;
 *     for( j = 0; j < 8; j++ )
 *     {
 *         if( CRC * 1L )
 *             CRC = 0xEDB88320 ^ (CRC >> 1) ;
 *         else
 *             CRC = (CRC >> 1);
 *     }
 *     CRC_TABLE[i] = CRC;
 * }
 */ 
const stlUInt32 strCrc32Table[256] =
{
    /* 0 ~ 49 */
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419,
    0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4,
    0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07,
    0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
    0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856,
    0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4,
    0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3,
    0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a,
    /* 50 ~ 99 */
    0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599,
    0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190,
    0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
    0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e,
    0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed,
    0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3,
    0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
    /* 100 ~ 149 */
    0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a,
    0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5,
    0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010,
    0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17,
    0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6,
    0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615,
    0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344,
    0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    /* 150 ~ 199 */
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a,
    0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1,
    0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c,
    0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef,
    0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe,
    0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31,
    0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c,
    0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    /* 200 ~ 249 */
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b,
    0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1,
    0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
    0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278,
    0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7,
    0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66,
    0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605,
    0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8,
    /* 250 ~ 255 */
    0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b,
    0x2d02ef8d
};

/**
 * @brief sha1을 위한 내부 연산 함수
 * @param[in]  sState       sState
 * @param[in]  sBuffer      sBuffer
 * @param[out] aErrorStack  에러 스택
 */
stlStatus strSha1Transform( stlUInt32          sState[5],
                            stlUInt8           sBuffer[64],
                            stlErrorStack    * aErrorStack)
{
    /*
     * a,b,c,d,e 의 변수는 보기에 prefix없는것이 나을것 같아서
     * prefix 붙이지 않았음.
     */
    stlUInt32            a;
    stlUInt32            b;
    stlUInt32            c;
    stlUInt32            d;
    stlUInt32            e;
    strByte64Quad16    * sBlock;

    sBlock = (strByte64Quad16*)sBuffer;
    /* Copy context->sState[] to working vars */
    a = sState[0];
    b = sState[1];
    c = sState[2];
    d = sState[3];
    e = sState[4];
    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
    R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
    R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
    R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
    R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
    R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
    R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
    R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
    R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
    R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
    R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
    R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
    R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
    R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
    R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
    R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
    R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
    R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
    R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
    R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
    /* Add the working vars back into context.sState[] */
    sState[0] += a;
    sState[1] += b;
    sState[2] += c;
    sState[3] += d;
    sState[4] += e;
    /* Wipe variables */
    a = b = c = d = e = 0;

    return STL_SUCCESS;
}

/**
 * @brief sha1를 사용하기 위해 context를 초기화 하는 함수.
 * @param[in]  aSha1        sha1 구조체
 * @param[out] aErrorStack  에러 스택
 */
stlStatus strSha1Init( strSha1Ctx         * aSha1,
                       stlErrorStack      * aErrorStack )
{
    /* SHA1 initialization constants */
    aSha1->mState[0] = 0x67452301;
    aSha1->mState[1] = 0xEFCDAB89;
    aSha1->mState[2] = 0x98BADCFE;
    aSha1->mState[3] = 0x10325476;
    aSha1->mState[4] = 0xC3D2E1F0;
    aSha1->mCount[0] = 0;
    aSha1->mCount[1] = 0;

    return STL_SUCCESS;
}

/**
 * @brief sha1 digest할 data를 입력한다.
 *        <BR> 이 함수는 중복 호출 가능하다.
 * @param[in]  aSha1        sha1 구조체
 * @param[in]  aValue       digest할 data
 * @param[in]  aValueLen    digest할 data의 길이
 * @param[out] aErrorStack  에러 스택
 */
stlStatus strSha1Update( strSha1Ctx       * aSha1,
                         stlUInt8         * aValue,
                         stlUInt64          aValueLen,
                         stlErrorStack    * aErrorStack )
{
    stlUInt32       i;
    stlUInt32       j;

    j = ( aSha1->mCount[0] >> 3 ) & 63;
    if( ( aSha1->mCount[0] += aValueLen << 3 ) < ( aValueLen << 3 ) )
    {
        aSha1->mCount[1]++;
    }
    aSha1->mCount[1] += ( aValueLen >> 29 );
    if( ( j + aValueLen ) > 63)
    {
        stlMemcpy( &aSha1->mBuffer[j], aValue, ( i = 64-j ) );
        STL_TRY( strSha1Transform( aSha1->mState, aSha1->mBuffer, aErrorStack )
                 == STL_SUCCESS );
        for ( ; i + 63 < aValueLen; i += 64 ) {
            STL_TRY( strSha1Transform( aSha1->mState, &aValue[i], aErrorStack )
                     == STL_SUCCESS );
        }
        j = 0;
    }
    else
    {
        i = 0;
    }

    stlMemcpy( &aSha1->mBuffer[j], &aValue[i], aValueLen - i );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief sha1 digest된 결과값을 output한다.
 * @param[in]  aSha1        sha1 구조체
 * @param[out] aResult      digest된 값이 저장될 버퍼.
 *                          <BR> 여기에는 STR_DIGEST_SIZE(20)byte의
 *                          <BR> 변수가 들어와야 한다.
 * @param[out] aErrorStack  에러 스택
 */
stlStatus strSha1Final( strSha1Ctx        * aSha1,
                        stlUInt8            aResult[STR_DIGEST_SIZE],
                        stlErrorStack     * aErrorStack )
{
    stlUInt32   i;
    stlUInt8    sFinalCount[8];

    for( i = 0; i < 8; i++ ) {
        sFinalCount[i] = (stlUInt8)( ( aSha1->mCount[(i >= 4 ? 0 : 1)]
         >> ( ( 3 - ( i & 3 ) ) * 8 ) ) & 255 );  /* Endian independent */
    }
    STL_TRY( strSha1Update( aSha1, (stlUInt8 *)"\200", 1, aErrorStack )
             == STL_SUCCESS );
    while( ( aSha1->mCount[0] & 504 ) != 448 ) {
        STL_TRY( strSha1Update( aSha1, (stlUInt8 *)"\0", 1, aErrorStack )
                 == STL_SUCCESS );
    }
    /* Should cause a SHA1_Transform() */
    STL_TRY( strSha1Update( aSha1, sFinalCount, 8, aErrorStack )
             == STL_SUCCESS );
    for( i = 0; i < SHA1_DIGEST_LENGTH; i++ ) {
        aResult[i] = (stlUInt8)( ( aSha1->mState[i>>2] >> ( ( 3 - ( i & 3 ) ) * 8 ) ) & 255 );
    }

    /* Wipe variables */
    i = 0;

    /*
     * 필요없을것 같아서  주석처리함.
    stlMemset(aSha1->mBuffer, 0, SHA1_BLOCK_LENGTH);
    stlMemset(aSha1->mState, 0, SHA1_DIGEST_LENGTH);
    stlMemset(aSha1->mCount, 0, 8);
    stlMemset(&sFinalCount, 0, 8);
     */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief sha1 input에 대한 digest된 결과값을 output한다.
 * @param[in]  aValue       digest할 data
 * @param[in]  aValueLen    digest할 data의 길이
 * @param[out] aResult      digest된 값이 저장될 버퍼.
 *                          <BR> 여기에는 STR_DIGEST_SIZE(20)byte의
 *                          <BR> 변수가 들어와야 한다.
 * @param[out] aErrorStack  에러 스택
 */
stlStatus strDigest( stlUInt8           * aValue,
                     stlUInt64            aValueLen,
                     stlUInt8             aResult[STR_DIGEST_SIZE],
                     stlErrorStack      * aErrorStack )
{
    /*
     * sSha1 동적할당 하지 않고 stack변수로 사용했음.
     * 맞는지 확인 필요함.
     */
    strSha1Ctx      sSha1;

    STL_PARAM_VALIDATE( aValue != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aValueLen > 0, aErrorStack );

    STL_TRY( strSha1Init( &sSha1, aErrorStack ) == STL_SUCCESS );
    STL_TRY( strSha1Update( &sSha1, aValue, aValueLen, aErrorStack )
             == STL_SUCCESS );
    STL_TRY( strSha1Final( &sSha1, aResult, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Data Input에 대한 Crc32 값을 계산한다.
 * @param [in]     aData            Crc32 계산되는 Data
 * @param [in]     aSize            Data의 Size
 * @param [out]    aCrc32           Crc32 초기값이자 반환되는 값
 * @param [in,out] aErrorStack      에러 스택
 */ 
stlStatus strCrc32( void          * aData,
                    stlInt32        aSize,
                    stlUInt32     * aCrc32,
                    stlErrorStack * aErrorStack )
{
    stlChar    * sBlock;
    stlInt32     sTableIdx;
    stlInt32     sIdx;
    stlUInt32    sCrc32;

    sBlock = (stlChar *) aData;
    sCrc32 = *aCrc32;

    for( sIdx = 0; sIdx < aSize; sIdx++ )
    {
        sTableIdx = ( (stlInt32) ( sCrc32 ^ *sBlock ) & 0xFF );
        sBlock++;
        sCrc32 = strCrc32Table[ sTableIdx ] ^ ( sCrc32 >> 8 );
    }

    *aCrc32 = sCrc32;
    
    return STL_SUCCESS;
}

/** @} */
